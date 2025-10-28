// This layout is based on your provided examples
layout(local_size_x = 10, local_size_y = 10) in;

// --- Assumed Built-in/Provided Variables ---
// vec2 UV: The current pixel coordinate (e.g., 0 to 1920, 0 to 1080)
// float TIME: Time in seconds for animation
// out vec4 VALUE: The final output color for the pixel

// --- Uniforms ---
uniform vec2 RESOLUTION; // Screen dimensions (e.g., 1920, 1080)

// --- "Physics" & Scene Constants ---
const float R_s = 0.5;                     // Schwarzschild Radius (Event Horizon) - REDUCED FURTHER
const float SHADOW_RADIUS = R_s * 2.59807; // The "Black Hole Shadow" (sqrt(27)/2 * R_s)
const float LENSING_STRENGTH = 1.5;        // "G*M" for lensing
const float CHECKER_SIZE = 2.0;            // Size of the checkerboard tiles

// --- Accretion Disk Constants ---
const float SPIN_A = 0.95; // Spin parameter (0=static, ~0.99=max)
// ISCO (Innermost Stable Circular Orbit) is now asymmetric
const float DISK_ISCO_PRO = R_s * 1.9;    // Prograde ISCO (closer, for SPIN_A=0.95)
const float DISK_ISCO_RETRO = R_s * 4.5;  // Retrograde ISCO (further, for SPIN_A=0.95)
const float DISK_OUTER_R = 5.0;           // Outer radius of the disk
const float DISK_THICKNESS = 0.05;        // How thick the disk is
const float DISK_NOISE_SCALE = 5.0;       // How "zoomed in" the plasma noise is
const float DISK_DENSITY_SCALE = 15.0;    // How dense the disk plasma is (affects visibility)

// --- Raytracing Constants ---
const int MAX_STEPS = 128;     // Max iterations per ray
const float MAX_DIST = 100.0;  // Max view distance
const float SURF_DIST = 0.002; // Precision for hitting a hard surface
const float VOL_STEP_MULT = 0.5; // Multiplier for volumetric step size

// --- Material IDs ---
const float MAT_SKY = 0.0;
const float MAT_SHADOW = 1.0; 
const float MAT_GROUND = 2.0;
// MAT_ACCRETION_DISK is now handled volumetrically, so no longer a hard surface ID

// --- Black Hole Position ---
const vec3 BLACK_HOLE_CENTER = vec3(0.0, 4.0, 0.0); // Moved higher above the plane

// --- SDF Helper Functions (return distance) ---

// Accretion Disk: p = point (relative to black hole)
// This now describes the *volume* of the disk, not a solid surface.
float sdAccretionDisk(vec3 p) {
    float r = length(p.xz); // Distance from Y axis
    
    // Asymmetric ISCO for Kerr Black Hole
    float side_interpolant = (p.x / max(r, 0.01) + 1.0) * 0.5; // 0=retrograde, 1=prograde
    float isco_radius = mix(DISK_ISCO_RETRO, DISK_ISCO_PRO, side_interpolant);

    float d_ring_inner = r - isco_radius;
    float d_ring_outer = r - DISK_OUTER_R;
    
    float d_ring = max(d_ring_inner, -d_ring_outer);
    float d_thickness = abs(p.y) - DISK_THICKNESS;
    
    // For a volume, we want to know how deep inside the disk we are.
    // Return distance to nearest boundary of the disk *volume*.
    return max(d_ring, d_thickness);
}


// --- Scene Map Function ---
// Returns vec2(signed_distance, material_id)
// This finds the closest *hard* object in the scene to point 'p'.
// The accretion disk is now volumetric and NOT included here.
vec2 map(vec3 p) {
    vec3 p_bh = p - BLACK_HOLE_CENTER;

    // 1. Black Hole Shadow
    vec2 res = vec2(length(p_bh) - SHADOW_RADIUS, MAT_SHADOW);
    
    // 2. Ground Plane
    vec2 ground = vec2(p.y, MAT_GROUND); // Plane at y=0
    if (ground.x < res.x) res = ground;
    
    return res;
}
// --- Procedural Noise Functions ---
// (Used to create the plasma texture for the disk)

// Simple pseudo-random number generator
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

// 3D hash for starfield
float hash3(vec3 p) {
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 19.19);
    return fract((p.x + p.y) * p.z);
}


// 2D Noise function
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Smoothstep
    
    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Fractal Brownian Motion (FBM) - to make "cloudy" plasma
float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    mat2 m = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5)); // Rotation matrix
    for (int i = 0; i < 4; i++) {
        v += a * noise(p);
        p = m * p * 2.0;
        a *= 0.5;
    }
    return v;
}


// --- Lighting & Shading ---

// 1. Get Sky Color
vec3 getSky(vec3 rd) {
    float sun = max(0.0, dot(rd, normalize(vec3(0.5, 0.2, 0.5))));
    vec3 col = vec3(0.1, 0.1, 0.3); // Deep space
    col = mix(col, vec3(0.4, 0.3, 0.3), pow(sun, 2.0)); // Sun glow
    col += vec3(0.5, 0.3, 0.2) * pow(sun, 4.0);
    col = mix(col, vec3(0.2, 0.3, 0.5), 0.5 - rd.y * 0.5); // Horizon

    // Procedural Starfield
    vec3 star_rd = normalize(rd) * 100.0;
    float star_hash = hash3(floor(star_rd));
    
    float star_brightness = 0.0;
    if (star_hash > 0.999) {
        star_brightness = (star_hash - 0.999) / (1.0 - 0.999);
        star_brightness = pow(star_brightness * 0.9 + 0.1, 10.0);
    }
    
    float star_color_hash = hash3(floor(star_rd) + 0.5);
    vec3 starColor = mix(vec3(1.0, 0.8, 0.5), vec3(0.8, 0.9, 1.0), star_color_hash);
    
    col += star_brightness * starColor * 5.0;
    col += hash3(floor(rd * 300.0)) * 0.02;

    return col;
}

// 2. Get Material Color (for hard surfaces)
vec3 getMaterial(vec3 p, float matID) {
    if (matID == MAT_GROUND) {
        if (mod(floor(p.x / CHECKER_SIZE) + floor(p.z / CHECKER_SIZE), 2.0) > 0.5) {
            return vec3(1.0); // White
        }
        return vec3(0.2); // Dark grey
    }
    if (matID == MAT_SHADOW) return vec3(0.0, 0.0, 0.0); // Black
    
    return vec3(1.0); // Default
}

// 3. Get Surface Normal (only for hard surfaces)
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        map(p + e.xyy).x - map(p - e.xyy).x,
        map(p + e.yxy).x - map(p - e.yxy).x,
        map(p + e.yyx).x - map(p - e.yyx).x
    ));
}

// 4. Get Emission/Lighting for Accretion Disk Volume
vec3 getAccretionDiskEmission(vec3 p_world, vec3 rayDir) {
    vec3 p_bh = p_world - BLACK_HOLE_CENTER;
    float r = length(p_bh.xz);
    float angle = atan(p_bh.x, p_bh.z);
    
    // 1. Procedural Noise for plasma texture
    float noise_coord = angle * 2.0 + TIME * 0.5;
    float n = fbm(vec2(noise_coord, r * 0.5) * DISK_NOISE_SCALE);
    n = smoothstep(0.3, 0.7, n); // Sharpen the noise
    
    // 2. Color Gradient (Hotter inside, asymmetric ISCO)
    float side_interpolant = (p_bh.x / max(r, 0.01) + 1.0) * 0.5;
    float isco_radius = mix(DISK_ISCO_RETRO, DISK_ISCO_PRO, side_interpolant);
    float temp = smoothstep(DISK_OUTER_R, isco_radius, r); // 0.0=cold, 1.0=hot
    
    vec3 color = mix(vec3(1.0, 0.1, 0.0), vec3(1.0, 0.8, 0.2), temp); // Red -> Yellow
    
    // 3. Relativistic Beaming (Doppler Effect)
    // Assume tangential velocity (counter-clockwise)
    vec3 velDir = normalize(cross(vec3(0,1,0), normalize(p_bh)));
    float doppler = max(0.0, dot(velDir, -rayDir));
    
    // 4. More Accurate Orbital Redshift (Gravitational + Time Dilation)
    float gravitational_redshift = sqrt(1.0 - (1.5 * R_s) / r);
    gravitational_redshift = max(gravitational_redshift, 0.05); // Clamp it to avoid NaN/too dark
    
    // Apply redshift: dims the light and shifts color towards red
    color = mix(vec3(1.0, 0.0, 0.0), color, gravitational_redshift * 0.8 + 0.2);

    // 5. Combine Brightness Effects
    float doppler_brightness = pow(1.0 + doppler, 3.0);
    float final_brightness = doppler_brightness * gravitational_redshift;
    
    // Final Emissive Color for this point in the volume
    return color * (n * 0.5 + 0.5) * final_brightness * 0.8; // *0.8 for overall disk dimming
}

// 5. Get Lighting for hard surfaces
vec3 getSurfaceLighting(vec3 p, float matID, vec3 rayDir) {
    vec3 color = getMaterial(p, matID);
    
    vec3 lightPos = vec3(10.0, 15.0, -10.0);
    vec3 lightDir = normalize(lightPos - p);
    vec3 normal = getNormal(p);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 ambient = vec3(0.15);
    
    vec3 viewDir = -rayDir;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    return ambient + color * diff + vec3(1.0) * spec;
}


// --- Main Compute Function ---
void Compute() {
    vec2 uv = (UV.xy - 0.5 * RESOLUTION.xy) / RESOLUTION.y;

    float camAngle = TIME * 0.2;
    float camDist = 15.0; 
    
    vec3 ro = vec3(sin(camAngle) * camDist, 5.0, cos(camAngle) * camDist); 
    vec3 target = BLACK_HOLE_CENTER; 

    vec3 fwd = normalize(target - ro);
    vec3 right = normalize(cross(fwd, vec3(0, 1, 0)));
    vec3 up = normalize(cross(right, fwd));

    vec3 rd = normalize(fwd + right * uv.x + up * uv.y);

    vec3 p = ro;       // Current position of the ray
    vec3 rayDir = rd;  // Current direction of the ray
    
    vec3 final_color = vec3(0.0);
    float accumulated_alpha = 0.0; // Total accumulated opacity
    
    for (int i = 0; i < MAX_STEPS; i++) {
        // --- Hard Surface Hit Detection ---
        vec2 res = map(p); // 'map' only contains hard surfaces (BH shadow, ground)
        float dist_to_hard_surface = res.x;
        float matID = res.y;
        
        // 1. Did we hit a hard object *before* max distance or full opacity?
        if (dist_to_hard_surface < SURF_DIST) {
            if (matID == MAT_SHADOW) {
                // If we hit the black hole shadow, it fully absorbs all remaining light
                final_color += vec3(0.0) * (1.0 - accumulated_alpha);
            } else {
                // If we hit the ground plane, render it and stop
                final_color += getSurfaceLighting(p, matID, rayDir) * (1.0 - accumulated_alpha);
            }
            accumulated_alpha = 1.0; // Fully opaque
            break;
        }
        
        // 2. Escaped to space?
        if (length(p - ro) > MAX_DIST) {
            break; 
        }
        
        // --- Volumetric Accretion Disk Sampling ---
        vec3 p_bh_local = p - BLACK_HOLE_CENTER;
        float dist_to_disk_volume = sdAccretionDisk(p_bh_local);
        
        // Calculate step size for this iteration
        // Adaptive step: smaller steps near BH and disk, larger elsewhere
        float current_step_size = max(0.01, min(dist_to_hard_surface * 0.5, length(p_bh_local) * 0.1));
        
        // If we are *inside* or *near* the accretion disk, make steps smaller
        if (dist_to_disk_volume < DISK_THICKNESS * 2.0 && length(p_bh_local.xz) > DISK_ISCO_PRO * 0.5) {
            current_step_size = min(current_step_size, DISK_THICKNESS * VOL_STEP_MULT);
            current_step_size = min(current_step_size, dist_to_disk_volume * 0.5 + 0.01);
        }

        // Apply lensing BEFORE moving the ray
        float dist_to_bh = length(p - BLACK_HOLE_CENTER);
        float r2 = dist_to_bh * dist_to_bh;
        float newtonian_a = LENSING_STRENGTH / r2;
        float gr_correction_a = LENSING_STRENGTH * 1.5 * R_s / (r2 * dist_to_bh); 
        float gravity = newtonian_a + gr_correction_a;
        vec3 g_vec = -normalize(p - BLACK_HOLE_CENTER) * gravity * current_step_size; // a * dt
        rayDir = normalize(rayDir + g_vec);

        // --- Sample the disk volume ---
        // 'density_factor' determines how opaque and bright a segment of the disk is
        // We use smoothstep to create a falloff from the disk boundaries
        float density_factor = 1.0 - smoothstep(0.0, DISK_THICKNESS * 0.8, dist_to_disk_volume);
        
        if (density_factor > 0.0) {
            // Get the light emitted by this "slice" of plasma
            vec3 emission = getAccretionDiskEmission(p, rayDir);
            
            // Calculate how much light this slice contributes and how much it blocks
            float alpha_step = density_factor * current_step_size * DISK_DENSITY_SCALE;
            
            // Accumulate color and alpha using standard volumetric rendering equation
            final_color += emission * alpha_step * (1.0 - accumulated_alpha);
            accumulated_alpha += alpha_step;
            
            // If we're fully opaque, stop accumulating disk light
            if (accumulated_alpha > 0.99) {
                accumulated_alpha = 1.0;
                break; 
            }
        }
        
        // Move the ray
        p += rayDir * current_step_size;
    }
    
    // 6. Add Sky if background is still visible
    // The accumulated_alpha handles how much of the sky is still seen
    final_color += getSky(rayDir) * (1.0 - accumulated_alpha);
    
    // Final Output
    VALUE = vec4(final_color, 1.0);
}

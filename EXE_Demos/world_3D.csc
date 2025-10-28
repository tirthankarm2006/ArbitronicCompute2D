// A single-file 3D ray-marched scene
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// --- Engine Variables ---
// Based on your errors, we must declare RESOLUTION...
uniform vec2 RESOLUTION;
// ...but UV, TIME, and VALUE are provided by the engine.
// uniform vec2 UV;
// uniform float TIME;
// out vec4 VALUE;

// --- Utility Functions ---

// Smooth Minimum: This is the magic that makes objects blend.
float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0);
    return min(a, b) - h*h*h / (6.0*k*k);
}

// --- 3D Primitives (Signed Distance Functions) ---

// Distance to a sphere at 'p' with radius 'r'
float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

// Distance to an infinite plane at y=0
float sdPlane(vec3 p) {
    return p.y;
}

// --- Scene Definition ---

// This function defines the entire 3D scene.
// It returns the shortest distance from point 'p' to any object.
float map(vec3 p) {
    // 1. The Ground
    float ground = sdPlane(p);

    // 2. A bouncing "metaball"
    // We'll use smin to merge two spheres.
    
    // Sphere 1: Bounces up and down
    float bounce = 0.5 + 0.5 * abs(sin(TIME * 1.5));
    vec3 sphere1_pos = p - vec3(0.0, bounce, 0.0);
    float sphere1 = sdSphere(sphere1_pos, 0.6);

    // Sphere 2: Orbits the first sphere
    vec3 sphere2_pos = p - vec3(0.7 * cos(TIME * 2.0), bounce, 0.7 * sin(TIME * 2.0));
    float sphere2 = sdSphere(sphere2_pos, 0.4);

    // Blend the two spheres into one "blob"
    float metaball = smin(sphere1, sphere2, 0.4);

    // Return the closest object (ground or metaball)
    return min(ground, metaball);
}

// --- Ray Marching Functions ---

// Calculate the surface normal (for lighting)
vec3 getNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    ));
}

// The main ray-marching loop
float rayMarch(vec3 ro, vec3 rd) {
    float d = 0.0; // Total distance marched
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + rd * d;
        float ds = map(p); // Distance from p to the scene
        d += ds;
        
        // We're close enough to hit, or we're too far
        if (ds < 0.001 || d > 100.0) break;
    }
    return d;
}

// --- Camera Setup ---

// Creates a "look-at" camera
mat3 setCamera(vec3 ro, vec3 ta, float up) {
    vec3 f = normalize(ta - ro);
    vec3 r = normalize(cross(vec3(0, up, 0), f));
    vec3 u = cross(f, r);
    return mat3(r, u, f);
}

// --- Main Compute Function ---

void Compute() {
    // 1. Setup screen coordinates (aspect-ratio corrected)
    vec2 p = (2.0 * UV - RESOLUTION) / RESOLUTION.y;

    // 2. Setup Camera
    // Orbiting camera
    vec3 ro = vec3(2.5 * sin(TIME * 0.3), 1.5, 2.5 * cos(TIME * 0.3));
    vec3 ta = vec3(0.0, 0.5, 0.0); // Target (looking at the metaball)
    
    // Get camera orientation
    mat3 cam = setCamera(ro, ta, 1.0);
    // Get ray direction for this pixel
    vec3 rd = cam * normalize(vec3(p, 1.5)); // 1.5 = zoom

    // 3. March!
    float d = rayMarch(ro, rd);

    // 4. Shading and Coloring
    vec3 col = vec3(0.0); // Default background color

    if (d < 100.0) { // We hit something
        vec3 pos = ro + rd * d; // Hit position
        vec3 nor = getNormal(pos); // Surface normal
        
        // Simple directional lighting
        vec3 lightDir = normalize(vec3(0.5, 1.0, -0.5));
        float diff = max(dot(nor, lightDir), 0.0);
        
        // Add a bit of ambient light
        float ambient = 0.3;
        
        // --- Color the scene ---
        if (pos.y < 0.01) { // It's the ground plane
            // Create a checkerboard pattern
            float f = mod(floor(pos.x * 2.0) + floor(pos.z * 2.0), 2.0);
            vec3 groundColor = vec3(0.1 + f * 0.3); // Dark/light gray
            col = groundColor * (diff + ambient);
        } else { // It's the metaball
            vec3 metaballColor = vec3(0.8, 0.3, 0.1); // Fiery orange
            col = metaballColor * (diff + ambient);
        }
    } else {
        // We hit nothing (sky)
        col = vec3(0.4, 0.6, 0.9) - 0.3 * rd.y; // Simple gradient sky
    }

    // 5. Final Output
    VALUE = vec4(col, 1.0);
}

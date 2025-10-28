// Compute-style GLSL: 2D Black Hole
// Adjusted: uses iResolution to compute UV from gl_GlobalInvocationID (pixel coords)
// Added INCLINATION uniform to tilt the camera relative to the accretion disk and
// DISK_THICKNESS to control apparent thickness when viewed edge-on.
// Assumes the host provides:
//   - float TIME           (global time in seconds)
//   - vec4 VALUE           (writeable output for this invocation) OR a host-wired VALUE variable
//   - vec2 iResolution     (render target size in pixels: width, height)
//   - float INCLINATION    (radians: 0.0 = face-on, PI/2 ~= edge-on)
//   - float DISK_THICKNESS (visual thickness multiplier, e.g. 1.0 default)
// If your host expects different names for those, replace them accordingly.

layout(local_size_x = 1, local_size_y = 1) in;

// host-provided resolution (pixels)
uniform vec2 iResolution;
// camera inclination: 0 = face-on, ~1.5708 = edge-on (parallel to disk)
uniform float INCLINATION;
// multiplier to control apparent vertical thickness of the accretion disk
uniform float DISK_THICKNESS;

// ---------------- utilities ----------------
float hash21(vec2 p){
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 23.45);
    return fract(p.x * p.y);
}
float noise2(vec2 p){
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash21(i + vec2(0.0,0.0));
    float b = hash21(i + vec2(1.0,0.0));
    float c = hash21(i + vec2(0.0,1.0));
    float d = hash21(i + vec2(1.0,1.0));
    vec2 u = f*f*(3.0-2.0*f);
    // bilinear with slight correction for smoothness
    float ab = mix(a, b, u.x);
    float cd = mix(c, d, u.x);
    return mix(ab, cd, u.y);
}
vec3 hsv2rgb(vec3 c){
    vec3 K = vec3(1.0, 2.0/3.0, 1.0/3.0);
    vec3 p = abs(fract(c.xxx + K) * 6.0 - 3.0);
    return c.z * mix(vec3(1.0), clamp(p - 1.0, 0.0, 1.0), c.y);
}

// small starfield generator
vec3 starfield(vec2 uv){
    vec2 p = uv * 180.0;
    vec2 ip = floor(p);
    vec2 fp = fract(p);
    float star = 0.0;
    for(int y=-1;y<=1;y++){
        for(int x=-1;x<=1;x++){
            vec2 cell = ip + vec2(x,y);
            float h = hash21(cell);
            float intensity = smoothstep(0.995, 0.9998, h); // very sparse
            if(intensity > 0.0){
                vec2 starPos = fract(vec2(hash21(cell+1.0), hash21(cell+2.0)));
                float d = length(fp - (vec2(x,y) + starPos));
                star += intensity * smoothstep(0.02, 0.002, d) * (0.6 + 0.4*hash21(cell+3.0));
            }
        }
    }
    float bg = 0.02 * noise2(uv * 2.5 + TIME*0.012);
    return vec3(bg + clamp(star, 0.0, 1.0));
}

// ---------------- main compute body ----------------
// Keep Compute() wrapper style for compatibility with hosts that expect that.
void Compute() {
    // compute pixel coords and normalized UV from iResolution
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    if (pix.x >= int(iResolution.x) || pix.y >= int(iResolution.y)) {
        VALUE = vec4(0.0);
        return;
    }
    vec2 UV = (vec2(pix) + 0.5) / iResolution;

    // normalized center (0..1)
    vec2 center = vec2(0.5, 0.5);

    // convert the provided normalized UV into centered coordinates
    // range roughly -1..1 (preserves square aspect assumption)
    vec2 p = (UV - center) * 2.0;

    // ---------------- Camera tilt (inclination)
    float inc = clamp(INCLINATION, 0.0, 1.57079632679);
    float cosInc = max(cos(inc), 1e-4);
    float sinInc = sin(inc);

    // Project the 2D image-plane coordinates to a tilted view by scaling the Y axis.
    vec2 p_tilt = vec2(p.x, p.y * cosInc);

    // background uses original UV (stars remain fixed relative to screen)
    vec3 col = starfield(UV);

    // Lensing parameters
    float bhMass = 0.9;   // strength of deflection
    int steps = 6;        // iterations for bending
    vec2 pL = p_tilt;
    float eps = 1e-4;

    // iterative toy-model gravitational deflection + small frame-drag swirl
    for(int i = 0; i < steps; ++i){
        float r = length(pL) + eps;
        float def = bhMass * (0.12 / (r*r + 0.02));
        pL -= normalize(pL + vec2(1e-6, 0.0)) * def;

        float swirl = 0.22 * sin(3.0 * TIME + float(i) * 0.7) / (1.0 + r * 5.5);
        vec2 tang = vec2(-pL.y, pL.x);
        pL += tang * swirl;
    }

    // For disk masking we want a coordinate in the disk's own plane (undo the Y compression)
    // so the disk keeps thickness even when the view is highly inclined.
    vec2 p_disk = vec2(pL.x, pL.y / cosInc); // unprojected into disk-plane coordinates
    float r_disk = length(p_disk);

    // event horizon (dark core)
    float r_horizon = 0.12;
    bool insideHorizon = (r_disk < r_horizon);

    // photon ring (bright narrow ring outside horizon) — widen slightly with inclination
    float ringRadius = r_horizon * 1.45;
    float ringWidth = 0.03 * (1.0 + 1.5 * sinInc); // make ring broader when edge-on
    float ring = exp(-pow((r_disk - ringRadius) / ringWidth, 2.0) * 4.0);

    // accretion disk (use unprojected disk coords so radius/thickness are stable)
    float ang = atan(p_disk.y, p_disk.x);
    float diskInner = r_horizon * 1.2;
    float diskOuter = 0.95;

    // thickness scaling — allow the disk to appear thicker as inclination approaches edge-on
    float thickScale = mix(1.0, 1.0 + 3.0 * DISK_THICKNESS, sinInc); // DISK_THICKNESS controls multiplier

    // radial mask computed in disk-plane coordinates (keeps width when tilted)
    float radialOuter = diskOuter * thickScale;
    float radialInner = diskInner * thickScale;
    float diskMaskRadial = smoothstep(radialOuter, radialOuter - 0.04*thickScale, r_disk) * (1.0 - smoothstep(radialInner, radialInner + 0.02*thickScale, r_disk));

    // vertical band: when nearly edge-on, the projection of disk thickness produces a bright band across the midline.
    // Use the tilted (projected) Y coordinate p_tilt.y to make a band visible on-screen.
    float bandWidth = 0.01 + 0.06 * DISK_THICKNESS * sinInc; // grows with inclination
    float band = exp(-pow(p_tilt.y / bandWidth, 2.0) * 4.0);

    // combine radial mask with the projected band — take the maximum so the band dominates when edge-on
    float diskMask = max(diskMaskRadial, band * smoothstep(0.0, 1.0, sinInc) * 1.2);

    float spiralFreq = 6.0;
    float tightness = 6.0;
    float spiral = 0.5 + 0.5 * sin(spiralFreq * ang + tightness * log(max(r_disk, 1e-5)) - 2.0 * TIME);
    float turb = noise2(vec2(ang * 2.0, r_disk * 8.0 + TIME * 0.36));
    float diskBrightness = clamp(spiral * 1.2 + 0.6 * turb, 0.0, 1.6) * diskMask;

    // disk colour: hot inner -> cool outer (based on disk-plane radius)
    float radialHeat = smoothstep(diskOuter * 1.0, diskInner * 1.0, r_disk);
    vec3 innerColor = vec3(1.0, 0.88, 0.65);
    vec3 outerColor = vec3(0.9, 0.22, 0.06);
    vec3 diskColor = mix(outerColor, innerColor, pow(radialHeat, 1.6));

    // subtle hue shift over time
    vec3 diskHSV = vec3(0.08 + 0.14 * sin(TIME * 0.2 + r_disk * 8.0), 0.9, 1.0);
    vec3 colorDiskTint = hsv2rgb(diskHSV);
    diskColor = mix(diskColor, colorDiskTint, 0.22);

    vec3 disk = diskColor * diskBrightness;

    // relativistic beaming approximation (one side brighter)
    float doppler = 0.5 + 0.5 * sin(ang - TIME * 1.1);
    float dopplerScale = mix(0.0, 1.0, sinInc);
    disk *= 0.7 + 0.65 * doppler * dopplerScale;

    // boost disk brightness a bit when edge-on so the band is visible
    disk *= 1.0 + 0.6 * sinInc;

    // combine accretion onto background
    col = mix(col, disk, clamp(diskBrightness * 1.0, 0.0, 1.0));

    // add photon ring (flickering)
    vec3 ringColor = vec3(1.0, 0.92, 0.75) * (1.0 + 0.5 * sin(TIME * 6.0 + ang * 9.5));
    col = mix(col, ringColor, clamp(ring * 1.6, 0.0, 1.0));

    // inside horizon: total black
    if (insideHorizon) {
        col = vec3(0.0);
    } else {
        // soft vignette to blend edges
        float vign = smoothstep(1.8, 0.6, length((UV - center) * 2.0));
        col *= vign;
    }

    // tone map + gamma-ish
    col = col / (col + vec3(1.0));
    col = pow(col, vec3(0.95));

    // final write to the environment-provided VALUE
    VALUE = vec4(clamp(col, 0.0, 1.0), 1.0);
}

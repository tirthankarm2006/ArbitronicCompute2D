
// A single-file ray-marched 4D Tesseract
layout(local_size_x = 10, local_size_y = 10) in;

// --- Engine Variables ---
uniform vec2 RESOLUTION;
// uniform vec2 UV;       // Provided
// uniform float TIME;    // Provided
// out vec4 VALUE;      // Provided

// --- Constants ---
const float LINE_THICKNESS = 0.015; // Our virtual line thickness
const float GLOW_FALLOFF = 0.05;   // How soft the glow is

// --- File-Scope Globals (for optimization) ---
// We will calculate these ONCE per pixel in Compute()
// and then READ them many times in map().
vec3 projectedVerts[16];

// --- 4D Math ---

// 4D Rotation matrix in the XW plane
mat4 rotXW(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat4(
        c, 0, 0, s,
        0, 1, 0, 0,
        0, 0, 1, 0,
       -s, 0, 0, c
    );
}

// 4D Rotation matrix in the YZ plane
mat4 rotYZ(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat4(
        1, 0, 0, 0,
        0, c,-s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    );
}

// Project a 4D point to 3D (Perspective projection)
vec3 project4Dto3D(vec4 p) {
    float distance = 2.0; // Distance from 4D camera
    float w = 1.0 / (distance - p.w);
    return vec3(p.x*w, p.y*w, p.z*w);
}

// --- 3D Math ---

// SDF for a 3D line segment
float sdLine(vec3 p, vec3 a, vec3 b) {
    vec3 pa = p - a;
    vec3 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

// --- Scene Definition ---

// These are the 16 vertices of a tesseract (a 4D hypercube)
const vec4 vertices[16] = vec4[](
    vec4(-1,-1,-1,-1), vec4( 1,-1,-1,-1), vec4(-1, 1,-1,-1), vec4( 1, 1,-1,-1),
    vec4(-1,-1, 1,-1), vec4( 1,-1, 1,-1), vec4(-1, 1, 1,-1), vec4( 1, 1, 1,-1),
    vec4(-1,-1,-1, 1), vec4( 1,-1,-1, 1), vec4(-1, 1,-1, 1), vec4( 1, 1,-1, 1),
    vec4(-1,-1, 1, 1), vec4( 1,-1, 1, 1), vec4(-1, 1, 1, 1), vec4( 1, 1, 1, 1)
);

// These are the 32 edges connecting the 16 vertices
const ivec2 edges[32] = ivec2[](
    ivec2(0,1), ivec2(0,2), ivec2(0,4), ivec2(1,3), ivec2(1,5), ivec2(2,3),
    ivec2(2,6), ivec2(3,7), ivec2(4,5), ivec2(4,6), ivec2(5,7), ivec2(6,7),
    ivec2(8,9), ivec2(8,10), ivec2(8,12), ivec2(9,11), ivec2(9,13), ivec2(10,11),
    ivec2(10,14), ivec2(11,15), ivec2(12,13), ivec2(12,14), ivec2(13,15), ivec2(14,15),
    ivec2(0,8), ivec2(1,9), ivec2(2,10), ivec2(3,11),
    ivec2(4,12), ivec2(5,13), ivec2(6,14), ivec2(7,15)
);

// This function defines the entire 3D scene.
// It returns the shortest distance from point 'p' to any object.
float map(vec3 p) {
    // This function is now very fast!
    // It just reads from the pre-calculated 'projectedVerts' array.
    float minDist = 1000.0;
    for (int i = 0; i < 32; i++) {
        vec3 a = projectedVerts[edges[i].x];
        vec3 b = projectedVerts[edges[i].y];
        minDist = min(minDist, sdLine(p, a, b));
    }
    
    // FIX for "dots": We subtract a thickness.
    // This makes the ray-marcher "hit" a fat tube instead of a thin line.
    return minDist - LINE_THICKNESS;
}

// --- Ray Marching Functions ---

// The main ray-marching loop
// Returns total distance marched
float rayMarch(vec3 ro, vec3 rd) {
    float d = 0.0;
    for (int i = 0; i < 80; i++) { // 80 steps is usually enough
        vec3 p = ro + rd * d;
        float ds = map(p);
        
        // Step forward, but not less than a tiny amount (to avoid stalling)
        d += max(0.001, ds * 0.8); // Step 80% of the distance for stability
        
        // If ds < 0.001, we are inside the "thick" line.
        if (ds < 0.001 || d > 100.0) break;
    }
    return d;
}

// --- Camera Setup ---
mat3 setCamera(vec3 ro, vec3 ta, float up) {
    vec3 f = normalize(ta - ro);
    vec3 r = normalize(cross(vec3(0, up, 0), f));
    vec3 u = cross(f, r);
    return mat3(r, u, f);
}

// --- Main Compute Function ---

void Compute() {
    // 1. --- OPTIMIZATION ---
    // Calculate all 16 projected vertices ONCE per pixel.
    mat4 R1 = rotXW(TIME * 0.4);
    mat4 R2 = rotYZ(TIME * 0.6);
    for (int i = 0; i < 16; i++) {
        vec4 v = vertices[i];
        v = R1 * v;
        v = R2 * v;
        projectedVerts[i] = project4Dto3D(v);
    }

    // 2. Setup screen coordinates
    vec2 p = (2.0 * UV - RESOLUTION) / RESOLUTION.y;

    // 3. Setup Camera
    vec3 ro = vec3(4.0 * sin(TIME * 0.15), 0.0, 4.0 * cos(TIME * 0.15));
    vec3 ta = vec3(0.0, 0.0, 0.0);
    
    mat3 cam = setCamera(ro, ta, 1.0);
    vec3 rd = cam * normalize(vec3(p, 1.5));

    // 4. March!
    float d = rayMarch(ro, rd);

    // 5. Shading and Coloring
    vec3 col = vec3(0.0);

    if (d < 100.0) { // We hit something
        vec3 pos = ro + rd * d;
        
        // To get the "true" distance (for the glow), we add back
        // the thickness we subtracted in map().
        float true_dist = map(pos) + LINE_THICKNESS;
        
        // Use smoothstep to create a soft, glowing line
        float intensity = 1.0 - smoothstep(0.0, GLOW_FALLOFF, true_dist);
        
        col = vec3(0.9, 0.7, 1.0) * intensity; // Bright lavender glow
    }
    
    // Add a dark blue background vignette
    col += vec3(0.0, 0.1, 0.2) * (1.0 - length(p * 0.7));

    // 6. Final Output
    VALUE = vec4(col, 1.0);
}
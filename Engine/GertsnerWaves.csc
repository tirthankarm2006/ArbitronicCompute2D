layout(local_size_x = 1, local_size_y = 1) in;

void drawCircle(vec2 centre, float radius, vec3 color){
   if((UV.x - centre.x)*(UV.x - centre.x) + (UV.y - centre.y)*(UV.y - centre.y) < radius){
       VALUE = vec4(color, 1.0);
   }
}

uniform vec2 k1;
uniform vec2 k2;
uniform vec2 k3;
uniform vec2 k4;
uniform vec2 k5;

uniform float phase1;
uniform float phase2;
uniform float phase3;
uniform float phase4;
uniform float phase5;

uniform float A1;
uniform float A2;
uniform float A3;
uniform float A4;
uniform float A5;

uniform float w1_scale; 
uniform float w2_scale; 
uniform float w3_scale; 
uniform float w4_scale; 
uniform float w5_scale;

float w0; 
float w1; 
float w2; 
float w3; 
float w4; 
float w5;

float w1_p; 
float w2_p; 
float w3_p; 
float w4_p; 
float w5_p;

void Compute(){
    VALUE = vec4(0.0, 0.0, 0.0, 1.0);

    //Calculating All w's
    w1 = sqrt(9.8 * sqrt(dot(k1, k1)) * w1_scale);
    w2 = sqrt(9.8 * sqrt(dot(k2, k2)) * w2_scale);
    w3 = sqrt(9.8 * sqrt(dot(k3, k3)) * w3_scale);
    w4 = sqrt(9.8 * sqrt(dot(k4, k4)) * w4_scale);
    w5 = sqrt(9.8 * sqrt(dot(k5, k5)) * w5_scale);

    //Calculating All w_p's
    w1_p = trunc(w1/w0) * w0;
    w2_p = trunc(w2/w0) * w0;
    w3_p = trunc(w3/w0) * w0;
    w4_p = trunc(w4/w0) * w0;
    w5_p = trunc(w5/w0) * w0;

    vec2 xz_node1 = (k1/sqrt(dot(k1, k1))) * A1 * sin(dot(k1, UV) - (w1_p * TIME) + phase1);
	vec2 xz_node2 = (k2/sqrt(dot(k2, k2))) * A2 * sin(dot(k2, UV) - (w2_p * TIME) + phase2);
	vec2 xz_node3 = (k3/sqrt(dot(k3, k3))) * A3 * sin(dot(k3, UV) - (w3_p * TIME) + phase3);
	vec2 xz_node4 = (k4/sqrt(dot(k4, k4))) * A4 * sin(dot(k4, UV) - (w4_p * TIME) + phase4);
	vec2 xz_node5 = (k5/sqrt(dot(k5, k5))) * A5 * sin(dot(k3, UV) - (w5_p * TIME) + phase5);

    float y_node1 = (A1 * cos(dot(k1, UV) - (w1 * TIME) + phase1));
	float y_node2 = (A2 * cos(dot(k2, UV) - (w2 * TIME) + phase2));
	float y_node3 = (A3 * cos(dot(k3, UV) - (w3 * TIME) + phase3));
	float y_node4 = (A4 * cos(dot(k4, UV) - (w4 * TIME) + phase4));
	float y_node5 = (A5 * cos(dot(k5, UV) - (w5 * TIME) + phase5));

    vec2 xz_value = xz_node1 + xz_node2 + xz_node3 + xz_node4 + xz_node5;
    float y_value = y_node1 + y_node2 + y_node3 + y_node4 + y_node5;

    VALUE = vec4(xz_value.x, y_value, xz_value.y, 1.0);
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

ivec2 m_centre = ivec2(500, 400);
vec3 color1 = vec3(0.6, 0.7, 0.3);
vec3 color2 = vec3(0.2, 0.5, 0.7);
vec3 color3 = vec3(0.4, 0.9, 0.1);
float speed1 = 2.5, speed2 = 1.5, speed3 = 4.0;

void drawCircle(vec2 centre, float radius, vec3 color);

void Compute(){
  drawCircle(m_centre + vec2(100, 100), 45000 * (1 + sin(speed1*TIME))/2, color1);
  drawCircle(m_centre + vec2(100, -100), 30000 * (1 + cos(speed2*TIME))/2, color2);
  drawCircle(m_centre+ vec2(-100, 100), 20000 * (2 + sin(speed1*TIME) + cos(speed2*TIME))/4, color3);
  drawCircle(m_centre+ vec2(-100, -100), 35000 * (2 + sin(speed2*TIME) + cos(speed1*TIME))/4, color3 + color1);
}

void drawCircle(vec2 centre, float radius, vec3 color){
   if((UV.x - centre.x)*(UV.x - centre.x) + (UV.y - centre.y)*(UV.y - centre.y) < radius){
       VALUE = vec4(color, 1.0);
   }

}

layout(local_size_x = 1, local_size_y = 1) in;

#define PI 3.14159265359;
uniform vec2 FOURIER_GRID_SIZE;
uniform vec2 PATCH_SIZE;

//Generates 2 random numbers based on Gaussian Distribution on the k values of the fourier amplitude height grids
vec2 GetRandomGaussianVec2(vec2 k);

void Compute(){
    //sum over all the values of k in the fourier grid k.x is from [-pi*N/Lx , pi*N/Lx], k.y is from [-pi*M/Ly , pi*M/Ly]
    for(){
        for(){
            
        }
    }
    VALUE = vec4(0.0, 0.0, 0.0, 1.0);
}

vec2 GetRandomGaussianVec2(vec2 k){ //k.x is from [-pi*N/Lx , pi*N/Lx], k.y is from [-pi*M/Ly , pi*M/Ly]
    vec2 i, f;
    float m_w;
    i.x = (k.x * PATCH_SIZE.x)/(FOURIER_GRID_SIZE.x * PI);// [-1, 1]
    i.y = (k.y * PATCH_SIZE.y)/(FOURIER_GRID_SIZE.y * PI);// [-1, 1]
    m_w = (i.x * i.x + i.y * i.y)/2;//[0, 1]
    m_w = sqrt((-2.0 * log(m_w))/m_w);
    f.x = m_w * i.x;
    f.y = m_w * i.y;
    return f;
}
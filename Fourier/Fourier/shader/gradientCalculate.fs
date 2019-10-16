#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

float xSobel[9] = float[9](
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
);

float ySobel[9] = float[9](
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
);


void main(){
    vec2 offset = 1.0 / vec2(textureSize(image, 0));
    float Gx = 0.0, Gy = 0.0;
    int index = 0;
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            float currentValue = texture(image, TexCoord + vec2(offset.x * i, offset.y * j)).r;
            Gx += xSobel[index] * currentValue;
            Gy += ySobel[index] * currentValue;
            index++;
        }
    }
    float G = sqrt(pow(Gx, 2) + pow(Gy, 2));
    FragColor = vec4(G, Gx, Gy, 1.0);;    
}

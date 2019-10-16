#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D loadedTexture;

float edgeKernel[9] = float[9](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);


const float scaleFactor = 1.0;

void main(){
    vec2 texSize = vec2(textureSize(loadedTexture, 0));
    vec3 sum = vec3(0.0, 0.0, 0.0);
    int index = 0;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            vec2 offset = vec2(float(i) / texSize.x, float(j) / texSize.y);
            sum += edgeKernel[index++] * texture(loadedTexture, TexCoord + offset).rgb;
        }
    }
    float test = (sum.r + sum.g + sum.b) / 3.0;
    test *= 2.0;
    if(test >= 0.001 && test <= 0.1)
        FragColor = vec4(scaleFactor * sum, 1.0);
    else{
        discard;
    }
    
//    FragColor = vec4(scaleFactor * sum, 1.0);
}

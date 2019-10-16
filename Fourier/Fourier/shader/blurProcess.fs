#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D loadedTexture;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // 高斯模糊

void main(){
    vec2 texSize = vec2(textureSize(loadedTexture, 0));
    vec2 offset = 1.0 / texSize;
    float result = 0.0;

    for(int i = 0; i < 5; i++){
        if(horizontal){
            result += texture(loadedTexture, TexCoord + vec2(offset.x * i, 0.0)).r * weight[i];
            result += texture(loadedTexture, TexCoord - vec2(offset.x * i, 0.0)).r * weight[i];
        }
        else{
            result += texture(loadedTexture, TexCoord + vec2(0.0, offset.y * i)).r * weight[i];
            result += texture(loadedTexture, TexCoord - vec2(0.0, offset.y * i)).r * weight[i];
        }
    }
    FragColor = vec4(result, result, result, 1.0);
}

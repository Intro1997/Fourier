#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image1;
uniform sampler2D image2;

void main(){
    //    FragColor = texture(boardColorBuffer, TexCoord);
    float val1 = texture(image1, TexCoord).r;
    float val2 = texture(image2, TexCoord).r;
    float Gx = texture(image2, TexCoord).g;
    float Gy = texture(image2, TexCoord).b;
    if(val2 > 0.0){
        FragColor = vec4(val2, 0.0, 0.0, 1.0);
    }
    if(Gx == 0.0)
        FragColor = vec4(0.0, 1.0 - Gx, 0.0, 1.0);
    if(Gy == 0.0)
        FragColor = vec4(0.0, 0.0, 1.0 - Gy, 1.0);
//    if(Gy == 0.0 && Gx == 0.0){
//        FragColor = vec4(1.0 - Gx, 1.0 - Gx, 0.0, 1.0);
//    }


    else
        FragColor = texture(image1, TexCoord);
}


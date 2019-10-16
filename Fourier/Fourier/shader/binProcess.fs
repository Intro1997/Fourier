#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

void main(){
    float currentVal = texture(image, TexCoord).r;
    if(currentVal > 0.1){
        vec2 stride = 1.0 / vec2(textureSize(image, 0));
        int connect = 0;
        for(int i = -1; i < 2; ++i){
            for(int j = -1; j < 2; ++j){
                vec2 offset = vec2(stride.x * float(i), stride.y * float(j));
                float val = texture(image, TexCoord + offset).r;
                if(val > 0.7)
                    connect++;
                if(connect >= 2){
                    break;
                }
            }
        }
        if(connect < 2)
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        else
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);

}


//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoord;
//
//uniform sampler2D image;
//
//void main(){
//    vec2 base = 1.0 / vec2(textureSize(image, 0));
//    float Gx = texture(image, TexCoord).g;
//    float Gy = texture(image, TexCoord).b;
//
//    float tangent;
//    vec2 offset;
//    if(Gy != 0){
//        tangent = Gx / Gy;
//        offset.x  = base.y * tangent;
//        offset.y  = base.y;
//        float divide = 1.0;
//        if(offset.x > base.x){
//            divide = offset.x / base.x;
//        }
//        offset /= divide;
//
//    }
//    else
//        offset = vec2(base.x, 0.0);
//
//
//
//    float O = texture(image, TexCoord).r;
//    float O1 = texture(image, TexCoord - offset).r;
//    float O2 = texture(image, TexCoord + offset).r;
//
//    if(O <= O1 || O <= O2)
//        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
//    else
//        FragColor = vec4(O, Gx, Gy, 1.0);
//}

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

void main(){
    vec2 base = 1.0 / vec2(textureSize(image, 0));
    float O1, O2;
    float O = texture(image, TexCoord).r;
    float Gx = texture(image, TexCoord).g;
    float Gy = texture(image, TexCoord).b;
    float G = texture(image, TexCoord).r;
    vec2 offsets[4] = vec2[4](
                              vec2(0.0, base.y),
                              vec2(base.x, 0.0),
                              vec2(base.x, base.y),
                              vec2(-base.x, base.y)
    );

    int index = -1;
    if(Gx == 0 && Gy == 0){
        O1 = O2 = O;
    }
    else if(Gx == 0.0){
        O1 = texture(image, TexCoord + offsets[0]).r;
        O2 = texture(image, TexCoord + -offsets[0]).r;
        index = 1;
    }
    else if(Gy == 0.0){
        O1 = texture(image, TexCoord + offsets[1]).r;
        O2 = texture(image, TexCoord + -offsets[1]).r;
        index = 2;
    }
    else if(Gx / Gy > 0.0){
        O1 = texture(image, TexCoord + offsets[2]).r;
        O2 = texture(image, TexCoord + -offsets[2]).r;
        index = 3;
    }
    else if(Gx / Gy < 0.0){
        O1 = texture(image, TexCoord + offsets[3]).r;
        O2 = texture(image, TexCoord + -offsets[3]).r;
        index = 4;
    }
    else{
        O1 = O2 = O;
    }

    // 当单纯用近似计算时，会有同一梯度方向上梯度相同但不为 0 的情况出现，这个时候如果将值相等的直接去除，会造成边缘不连续，因此将梯度方向旋转 90度
    // 之后再采样判断
    if(O == O1 || O == O2){
        if(index < 3 && index > 0){
            index -= 1;
            if(index == 1)
                index = 0;
            else
                index = 1;
            O1 = texture(image, TexCoord + offsets[index]).r;
            O2 = texture(image, TexCoord + -offsets[index]).r;
        }
        if(index >= 3){
            index -= 3;
            if(index == 1)
                index = 0;
            else
                index = 1;

            index += 2;
            O1 = texture(image, TexCoord + offsets[index]).r;
            O2 = texture(image, TexCoord + -offsets[index]).r;
        }
    }
    
    if(O < O1 || O < O2)
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else{
        if(G > 0.5){
            FragColor = vec4(O, Gx, Gy, 1.0);
        }
        else{
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }

    }
//    if(Gx != 0.0 && Gy == 0.0 && G != 0.0)
//        FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}

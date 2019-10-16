#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D image;

void main(){
    FragColor = texture(image, TexCoord);
    float avg = (FragColor.r + FragColor.g + FragColor.b);
    FragColor = vec4(avg, avg, avg, 1.0);
}

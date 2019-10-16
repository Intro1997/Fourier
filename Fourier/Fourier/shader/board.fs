#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D boardColorBuffer;

void main(){

    FragColor = texture(boardColorBuffer, TexCoord);
    float val = texture(boardColorBuffer, TexCoord).r;
    FragColor = vec4(val, val, val, 1.0);
	//FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
}


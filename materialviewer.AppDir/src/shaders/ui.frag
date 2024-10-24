#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 extraColor;

void main(){
    color = vec4(extraColor, 1.0) * texture(image, TexCoords);
    if(color.a < 0.01) discard;
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normalVec;
out vec2 texCoord;
out vec3 FragPos;

void main() {
    normalVec = normalize(mat3(transpose(inverse(model))) * aNormal);
    texCoord = aTexCoord;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
#version 330 core
in vec3 normalVec;
in vec3 texCoord;
in vec3 FragPos;

uniform samplerCube skybox;

uniform vec3 camPos;

out vec4 FragColor;


void main(){ 
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 reflectionDir = reflect(-viewDir, normalVec);

    vec3 envColor = texture(skybox, normalize(reflectionDir)).rgb;
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    vec4 ambient = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);

    float ambientAmount = 0.20;
    float diffuseAmount = 0.40;
    float specularAmount = 0.40;

    FragColor = vec4(envColor, 1.0);
    FragColor.a = 1.0;
}
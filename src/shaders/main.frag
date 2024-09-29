#version 330 core
in vec3 normalVec;
in vec3 texCoord;
in vec3 FragPos;

uniform sampler2D skybox;
const vec2 invAtan = vec2(0.1591, 0.3183);

uniform vec3 camPos;

out vec4 FragColor;

vec2 SampleSphericalMap(vec3 v){
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main(){ 
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 reflectionDir = reflect(-viewDir, normalVec);

    vec2 uv = SampleSphericalMap(normalize(reflectionDir));
    vec4 tex = texture(skybox, uv);
    vec4 ambient = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);

    float ambientAmount = 0.20;
    float diffuseAmount = 0.40;
    float specularAmount = 0.40;

    FragColor = tex;
    FragColor.a = 1.0;
}
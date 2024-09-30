#version 330 core
in vec3 normalVec;
in vec2 texCoord;
in vec3 FragPos;

uniform samplerCube irradianceMap;
uniform samplerCube envCubemap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfMap;

const float PI = 3.14159265359;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 camPos;

out vec4 FragColor;

vec3 fresnelShlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}
float GeometryShlickGGX(float NdotV, float roughness){
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float ggx1 = GeometryShlickGGX(NdotL, roughness);
    float ggx2 = GeometryShlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}
vec3 getNormalFromMap(){
    vec3 tangent = texture(normalMap, texCoord).xyz * 2.0 - 1.0;
    vec3 Q1 = dFdx(FragPos);
    vec3 Q2 = dFdy(FragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);
    vec3 N = normalize(normalVec);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangent);
}
void main(){ 
    vec3 albedo = pow(texture(albedoMap, texCoord).rgb, vec3(2.2));
    vec3 normal = getNormalFromMap();
    float metallic = texture(metallicMap, texCoord).r;
    float roughness = texture(roughnessMap, texCoord).r;
    float ao = texture(aoMap, texCoord).r;

    vec3 viewDir = normalize(camPos - FragPos);
    vec3 reflectionDir = reflect(-viewDir, normal);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 kS = fresnelShlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, normal).rgb;

    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectionDir, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfMap, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
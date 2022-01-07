#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct DirLight {
    vec3 direction;
    vec3 ambient;   
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;       
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;       
};

#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLight;

uniform vec3 viewPos;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometryShlickGGX(float NdotV, float roughness) {
    float r  = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometryShlickGGX(NdotV, roughness);
    float ggx1 = GeometryShlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

uniform sampler2D texture_albedo1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

vec3 CalcPointLight(int light_i, vec3 V, vec3 albedo, vec3 N, float metallic, float roughness, vec3 F0);

void main()
{   
    if (texture(texture_albedo1, fs_in.TexCoords).a < 0.5) discard;
    vec3 albedo     = texture(texture_albedo1, fs_in.TexCoords).rgb;
    vec3 N = texture(texture_normal1, fs_in.TexCoords).rgb;
    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space
    float metallic  = texture(texture_metallic1, fs_in.TexCoords).r;
    float roughness = texture(texture_metallic1, fs_in.TexCoords).g;
    float ao        = texture(texture_ao1, fs_in.TexCoords).r;

    vec3 V = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 1; i++) {
        Lo += CalcPointLight(i, V, albedo, N, metallic, roughness, F0);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/1.0));

    FragColor = vec4(color, 1.0);
}

vec3 CalcPointLight(int light_i, vec3 V, vec3 albedo, vec3 N, float metallic, float roughness, vec3 F0) {
        PointLight light = pointLights[light_i];
        vec3 L = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        vec3 H = normalize(V + L);
        float dist = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
        vec3 radiance = light.ambient * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        return (kD * albedo / PI + specular) * radiance * NdotL;
}
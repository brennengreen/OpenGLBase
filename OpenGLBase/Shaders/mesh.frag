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
    vec4 ShadowCoord;
} fs_in;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec3 diffuse;
    vec3 specular;       
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform samplerCube shadow_map;

uniform float far_plane;

#define NR_POINT_LIGHTS 1
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLight;

uniform vec3 viewPos;

uniform bool has_diff;
uniform bool has_spec;
uniform bool has_normal;
uniform bool has_height;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _diffuse, vec3 _specular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _diffuse, vec3 _specular);
float ShadowCalculation(vec3 fragPos);

void main()
{   
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 normal;
    if (has_diff) {
        diffuse = texture(texture_diffuse1, fs_in.TexCoords).rgb;
        if (texture(texture_diffuse1, fs_in.TexCoords).a < 0.5) discard;
    }
    if (has_spec) {
        specular = texture(texture_specular1, fs_in.TexCoords).rrr;
    }
    if (has_normal) {
        // obtain normal from normal map in range [0,1]
        vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
        // transform normal vector to range [-1,1]
    } else {
        normal = fs_in.Normal;
    }
    
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    //vec3 result = CalcDirLight(dirLight, normal, viewDir);
    vec3 result = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, fs_in.FragPos, viewDir, diffuse, specular);
    //result += CalcSpotLight(spotLight, normal, fs_in.FragPos, viewDir);

    float gamma = 2.2;//2.2;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(texture(texture_specular1, fs_in.TexCoords).rrr, 1.0);
    //FragColor.rgb = pow(result, vec3(1.0/gamma));
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLights[0].position;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 250.0;
    int samples = 750;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadow_map, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        //FragColor = vec4(vec3(closestDepth / far_plane), 1.0f);
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    //FragColor = vec4(vec3(shadow), 1.0);
    return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 _diffuse, vec3 _specular)
{
    // get diffuse color
    vec3 color = _diffuse;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    if (!has_spec) {
        return ((light.ambient * color) + (light.diffuse * diffuse));
    }
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = _specular * spec;
    // combine results
    return ((light.ambient * color) + (light.diffuse * diffuse) + (light.specular * specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 _diffuse, vec3 _specular)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // get diffuse color
    vec3 color = _diffuse;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    float shadow = ShadowCalculation(fs_in.FragPos);                    

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = _specular * spec;   
    // combine results
    vec3 this_ambient = light.ambient * color;
    vec3 this_diffuse = light.diffuse * diffuse;
    vec3 this_specular = light.specular * specular;
    this_ambient *= attenuation;
    this_diffuse *= attenuation;
    this_specular *= attenuation;


    return vec3((1.0f - shadow)*(this_specular + this_diffuse) + this_ambient);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = texture(texture_specular1, fs_in.TexCoords).rrr * spec;
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 this_ambient = light.ambient * color;
    vec3 this_diffuse = light.diffuse * diffuse;
    vec3 this_specular = light.specular * specular;
    this_ambient *= attenuation * intensity;
    this_diffuse *= attenuation * intensity;
    this_specular *= attenuation * intensity;
    return (this_ambient + this_diffuse + this_specular);
}

//float DirShadowCalculation() {
    //    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //    projCoords = projCoords * 0.5 + 0.5;
    //    float closestDepth = texture(shadow_map, projCoords.xy).r; 
    //    float currentDepth = projCoords.z;
    //    vec3 lightDir = normalize(dirLight.direction);
    //    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //    float shadow = 0.0;
    //    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    //    for(int x = -1; x <= 1; ++x)
    //    {
    //        for(int y = -1; y <= 1; ++y)
    //        {
    //            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
    //            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
    //        }    
    //    }
    //    shadow /= 9.0;
    //    
    //    if(projCoords.z > 1.0)
    //        shadow = 0.0;
    //        
    //    return shadow;
//}
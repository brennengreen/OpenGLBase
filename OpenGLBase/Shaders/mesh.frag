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

#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{          
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    
    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, fs_in.FragPos, viewDir);
    result += CalcSpotLight(spotLight, normal, fs_in.FragPos, viewDir);


    float gamma = 1.0;//2.2;
    FragColor = vec4(result, 1.0);
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = texture(texture_specular1, fs_in.TexCoords).rgb * spec;
    // combine results
    return ((light.ambient * color) + (light.diffuse * diffuse) + (light.specular * specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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

    vec3 specular = texture(texture_specular1, fs_in.TexCoords).rgb * spec;
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 this_ambient = light.ambient * color;
    vec3 this_diffuse = light.diffuse * diffuse;
    vec3 this_specular = light.specular * specular;
    this_ambient *= attenuation;
    this_diffuse *= attenuation;
    this_specular *= attenuation;
    //return (this_ambient + this_diffuse + this_specular);
    return vec3(this_specular + this_diffuse + this_ambient);
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

    vec3 specular = texture(texture_specular1, fs_in.TexCoords).rgb * spec;
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
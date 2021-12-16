#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform DirLight dirLight;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - N * dot(N, T));

    vs_out.Normal = N;
    vec3 B = normalize(normalMatrix * aBitangent);
    
    if (dot(cross(N, T), B) < 0.0) T *= -1.0;
    vs_out.Tangent = T;

    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * dirLight.direction;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
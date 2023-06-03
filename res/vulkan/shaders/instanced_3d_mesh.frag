#version 450

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 fragNormal;

layout(binding = 1) uniform sampler2D texSampler;

vec3 u_LightColor;
vec3 u_LightDirection;
vec3 u_ViewPos;
float u_LightIntensity;
float u_Shininess;

layout (location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    float u_LightIntensity = 1.5;
    float u_Shininess = 32.0;
	u_LightColor = vec3(1.0, 1.0, 1.0);
	u_LightDirection = vec3(-1.0, -1.0, -1.0);
	u_ViewPos = vec3(0.0, 0.0, 100.0);

    vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);
    vec3 diffuseMaterial = vec3(1.0, 1.0, 1.0);
    vec3 specularMaterial = vec3(0.5, 0.5, 0.5);

    vec3 normalizedLightDirection = normalize(u_LightDirection);
    vec3 normalizedViewDirection = normalize(u_ViewPos - fragPos);

    // Ambient component
    vec3 ambient = ambientMaterial;

    // Diffuse component
    vec3 diffuse = max(dot(fragNormal, normalizedLightDirection), 0.0) * diffuseMaterial;

    // Specular component
    vec3 reflectDir = reflect(-normalizedLightDirection, fragNormal);
    float spec = pow(max(dot(fragNormal, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_LightIntensity * spec * specularMaterial;

    vec3 result = (ambient + diffuse + specular) * texture(texSampler, v_TexCoord).rgb;
    fragColor = vec4(result, 1.0);
}
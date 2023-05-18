#version 450

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 fragNormal;

vec3 u_LightColor;
vec3 u_LightDirection;
vec3 u_ViewPos;

layout (location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec3 fragColor;

void main()
{
	u_LightColor = vec3(1.0, 1.0, 1.0);
	u_LightDirection = vec3(-1.0, -1.0, -1.0);
	u_ViewPos = vec3(0.0, 0.0, 1000.0);

    vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);
    vec3 diffuse = max(dot(fragNormal, -u_LightDirection), 0.0) * 
        u_LightColor * vec3(0.5, 0.5, 0.5);
    vec3 specular = pow(max(dot(reflect(u_LightDirection, fragNormal), 
        normalize(u_ViewPos - fragPos)), 0.0) * 0.7, 6.0) 
        * vec3(0.5, 0.5, 0.5)
        * 0.6;
    fragColor = diffuse + specular + ambient;
}
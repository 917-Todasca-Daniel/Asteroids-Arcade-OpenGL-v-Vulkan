#version 450

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 fragNormal;

layout(binding = 1) uniform sampler2D texSampler;

vec3 u_LightColor;
vec3 u_LightDirection;
vec3 u_ViewPos;

layout (location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	u_LightColor = vec3(1.0, 1.0, 1.0);
	u_LightDirection = vec3(-1.0, -1.0, -1.0);
	u_ViewPos = vec3(0.0, 0.0, 100.0);

    vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);
    vec3 diffuse = max(dot(fragNormal, -u_LightDirection), 0.0) * 
        u_LightColor * texture(texSampler, v_TexCoord).rgb;
    vec3 specular = pow(max(dot(reflect(u_LightDirection, fragNormal), 
        normalize(u_ViewPos - fragPos)), 0.0) * 0.7, 6.0) 
        * texture(texSampler, v_TexCoord).rgb
        * 0.6;
    fragColor = texture(texSampler, v_TexCoord);
}
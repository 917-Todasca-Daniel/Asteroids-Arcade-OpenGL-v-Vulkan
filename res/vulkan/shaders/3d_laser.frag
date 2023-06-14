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
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
#version 450

layout(binding = 0) uniform UniformBufferObject {
    float timelapse;
} ubo;

layout (location = 0) in vec3 position;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out float u_Time;

vec4 coord4d;

void main()
{
	coord4d = vec4(position, 1.0);
	gl_Position = coord4d;
	v_TexCoord = position.xy;
	u_Time = ubo.timelapse;
}
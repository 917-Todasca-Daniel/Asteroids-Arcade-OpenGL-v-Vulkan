#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

vec4 coord4d;

void main()
{
	coord4d = vec4(position, 1.0);
	gl_Position = coord4d;
	v_TexCoord = position.xy;
}
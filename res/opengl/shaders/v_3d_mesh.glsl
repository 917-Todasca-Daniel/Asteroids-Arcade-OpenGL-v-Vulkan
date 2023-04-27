#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform vec3 u_Origin;

out vec3 fragPos;
out vec3 fragNormal;

vec3 coord3d;

void main()
{
	mat4 model = mat4(1.0);

	coord3d = u_Origin + position;
    gl_Position = vec4(coord3d * 0.0005, 1.0);

	fragPos = position;
    fragNormal = mat3(transpose(inverse(model))) * normal;
}
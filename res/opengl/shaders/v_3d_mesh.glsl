#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 u_Projection[75];

out vec3 fragPos;
out vec3 fragNormal;
out vec2 v_TexCoord;

vec4 coord4d;

void main()
{
	mat4 model = mat4(1.0);

	coord4d = vec4(position, 1.0);
	gl_Position = u_Projection[gl_InstanceID] * coord4d;

	fragPos = position;
    fragNormal = mat3(transpose(inverse(model))) * normal;
	
	v_TexCoord = texCoord;
}
#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} ubo;

layout (location = 0) out vec3 fragPos;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec2 v_TexCoord;

vec4 coord4d;

void main()
{
	mat4 model = mat4(1.0);

	coord4d = vec4(position, 1.0);
	gl_Position = ubo.proj * coord4d;

	fragPos = position;
    fragNormal = mat3(transpose(inverse(model))) * normal;
	
	v_TexCoord = texCoord;
}
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

void main()
{
    gl_Position = vec4(position * 0.0005, 1.0);
}
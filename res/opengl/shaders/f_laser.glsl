#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 u_LightColor;
uniform vec3 u_LightDirection;
uniform vec3 u_ViewPos;

in vec2 v_TexCoord;

void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
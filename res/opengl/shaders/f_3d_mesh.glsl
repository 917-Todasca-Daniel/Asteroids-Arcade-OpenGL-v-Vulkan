#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 u_LightColor;
uniform vec3 u_LightDirection;
uniform vec3 u_ViewPos;

void main()
{
    vec3 ambient = 0.2 * vec3(1.0, 1.0, 1.0);
    vec3 diffuse = max(dot(fragNormal, -u_LightDirection), 0.0) * u_LightColor * vec3(1.0, 1.0, 1.0);
    vec3 specular = pow(max(dot(reflect(u_LightDirection, fragNormal), normalize(u_ViewPos - fragPos)), 0.0), 32.0) * vec3(1.0, 1.0, 1.0);
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
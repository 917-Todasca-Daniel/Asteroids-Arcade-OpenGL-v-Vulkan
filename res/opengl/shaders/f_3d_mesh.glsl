#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 u_LightColor;
uniform vec3 u_LightDirection;
uniform vec3 u_ViewPos;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
    vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);
    vec3 diffuse = max(dot(fragNormal, -u_LightDirection), 0.0) * 
        u_LightColor * texture(u_Texture, v_TexCoord).rgb;
    vec3 specular = pow(max(dot(reflect(u_LightDirection, fragNormal), 
        normalize(u_ViewPos - fragPos)), 0.0) * 0.7, 6.0) 
        * texture(u_Texture, v_TexCoord).rgb
        * 0.6;
    gl_FragColor = vec4(diffuse + specular + ambient, 1.0);
}
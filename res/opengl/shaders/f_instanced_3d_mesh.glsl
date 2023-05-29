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
    float u_LightIntensity = 1.5;
    float u_Shininess = 32.0;

    vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);
    vec3 diffuseMaterial = vec3(1.0, 1.0, 1.0);
    vec3 specularMaterial = vec3(0.5, 0.5, 0.5);

    vec3 normalizedLightDirection = normalize(u_LightDirection);
    vec3 normalizedViewDirection = normalize(u_ViewPos - fragPos);

    // Ambient component
    vec3 ambient = ambientMaterial;

    // Diffuse component
    vec3 diffuse = max(dot(fragNormal, normalizedLightDirection), 0.0) * diffuseMaterial;

    // Specular component
    vec3 reflectDir = reflect(-normalizedLightDirection, fragNormal);
    float spec = pow(max(dot(fragNormal, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_LightIntensity * spec * specularMaterial;

    vec3 result = (ambient + diffuse + specular) * texture(u_Texture, v_TexCoord).rgb;
    gl_FragColor = vec4(result, 1.0);
}
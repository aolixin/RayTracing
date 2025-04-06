#version 460 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos = vec3(0, 2, 0);
uniform vec3 viewPos;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.5, 0.5, 0.5);
uniform float ambientStrength = 0.1;
uniform float diffuseStrength = 0.8;
uniform float specularStrength = 0.5;
uniform int shininess = 32;

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal );
    FragColor = vec4(norm, 1.0);
    return;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), float(shininess));
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}    
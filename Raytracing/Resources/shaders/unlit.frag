#version 460 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor = vec3(0.5, 0.5, 0.5);

void main()
{
    FragColor = vec4(objectColor, 1.0);
}    
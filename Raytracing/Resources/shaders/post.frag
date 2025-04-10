#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

float exposure = 5.0;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    col = vec3(1.0) - exp(-color * exposure);
    col = pow(col, vec3(1.0/2.2));
    FragColor = vec4(col, 1.0);
} 
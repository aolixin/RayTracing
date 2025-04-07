#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

vec3 toneMapping(in vec3 c, float limit) {
    float luminance = 0.3 * c.x + 0.6 * c.y + 0.1 * c.z;
    return c * 1.0 / (1.0 + luminance / limit);
}


void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    col = col / (col + vec3(1.0));
    col = pow(col, vec3(1.0/2.2));
    FragColor = vec4(col, 1.0);
} 
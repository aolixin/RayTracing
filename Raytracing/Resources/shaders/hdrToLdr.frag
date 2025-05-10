#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D hdrTexture;

// Reinhard tone mapping
vec3 ReinhardToneMapping(vec3 color)
{
    return color / (1.0 + color);
}

// ACES tone mapping
vec3 ACESToneMapping(vec3 color)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    
    // 使用 ACES 色调映射
    vec3 mapped = ACESToneMapping(hdrColor);
    
    // Gamma 校正
    mapped = pow(mapped, vec3(1.0 / 2.2));
    
    FragColor = vec4(mapped, 1.0);
} 
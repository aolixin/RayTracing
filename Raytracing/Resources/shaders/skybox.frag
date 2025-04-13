#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube environmentMap;

void main()
{
    vec3 pos = vec3(localPos.x, -localPos.y, localPos.z);
    vec3 envColor = texture(environmentMap, pos).rgb;

    envColor = vec3(1.0) - exp(-envColor * 0.5);
    envColor = pow(envColor, vec3(1.0 / 2.2));

    FragColor = vec4(envColor, 1.0);
}
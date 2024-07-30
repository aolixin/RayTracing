#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    //vec3  normalizedWorldPos = normalize(WorldPos);
    vec3 envColor = texture(environmentMap, WorldPos).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    //FragColor = vec4(abs(envColor), 1.0);
    FragColor = vec4(vec3(0.0,0.5,0.0), 1.0);
}
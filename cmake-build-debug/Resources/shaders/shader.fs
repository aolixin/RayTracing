#version 330 core



// 声明一个结构体
struct Material {
    vec3 emissive;
    vec3 baseColor ;
    float subsurface;
    float metallic ;
    float specular;
    float specularTint;
    float roughness ;
    float anisotropic ;
    float sheen;
    float sheenTint ;
    float clearcoat;
    float clearcoatGloss ;
    float IOR;
    float transmission ;
};

// 光线求交结果
struct HitResult {
    bool isHit;             // 是否命中
    bool isInside;          // 是否从内部命中
    float distance;         // 与交点的距离
    vec3 hitPoint;          // 光线命中点
    vec3 normal;            // 命中点法线
    vec3 viewDir;           // 击中该点的光线的方向
    Material material;      // 命中点的表面材质
};


out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform Material material; // 声明一个结构体类型的uniform
uniform samplerBuffer triangles;
uniform samplerBuffer nodes;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(material.baseColor,1);
}
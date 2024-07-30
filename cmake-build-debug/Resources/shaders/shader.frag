#version 330 core



// ����һ���ṹ��
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

// �����󽻽��
struct HitResult {
    bool isHit;             // �Ƿ�����
    bool isInside;          // �Ƿ���ڲ�����
    float distance;         // �뽻��ľ���
    vec3 hitPoint;          // �������е�
    vec3 normal;            // ���е㷨��
    vec3 viewDir;           // ���иõ�Ĺ��ߵķ���
    Material material;      // ���е�ı������
};


out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform Material material; // ����һ���ṹ�����͵�uniform
uniform samplerBuffer triangles;
uniform samplerBuffer nodes;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(material.baseColor,1);
}
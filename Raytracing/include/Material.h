#pragma once

#include <glm/glm.hpp>
using namespace std;
using namespace glm;

struct Material_encoded {
    vec3 emissive = vec3(0, 0, 0);  // ��Ϊ��Դʱ�ķ�����ɫ
    vec3 baseColor = vec3(0, 0, 0);

    vec3 param1;
    vec3 param2;
    vec3 param3;
    vec3 param4;

};

class Material {
public:

    shared_ptr<Shader>shader;
    
    vec3 emissive = vec3(0, 0, 0);  // emissive color for light source
    vec3 baseColor = vec3(0, 0, 0); // base color for diffuse reflection
    float subsurface = 0.0;
    float metallic = 0.0;
    float specular = 0.5;

    float specularTint = 0.0;
    float roughness = 0.5;
    float anisotropic = 0.0;

    float sheen = 0.0;
    float sheenTint = 0.5;
    float clearcoat = 0.0;

    float clearcoatGloss = 1.0;
    float IOR = 1.0;
    float transmission = 0.0;

    Material() {
    }
    
    Material(const Shader& shader) {
        this->shader = make_shared<Shader>(shader);
    }

    Material_encoded encoded() {
        Material_encoded m;
        m.emissive = this->emissive;
        m.baseColor = this->baseColor;

        m.param1 = vec3(this->subsurface, this->metallic, this->specular);
        m.param2 = vec3(this->specularTint, this->roughness, this->anisotropic);
        m.param3 = vec3(this->sheen, this->sheenTint, this->clearcoat);
        m.param4 = vec3(this->clearcoatGloss, this->IOR, this->transmission);
        return m;
    }
};




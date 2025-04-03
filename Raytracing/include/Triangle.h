#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations
#define MAX_BONE_INFLUENCE 4


#include "Shader.h"
#include "Mesh.h"
#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

    Vertex() {
        this->Position = vec3(0, 0, 0);
        this->Normal = vec3(0, 0, 0);
        this->TexCoords = vec2(0, 0);
        this->Tangent = vec3(0, 0, 0);
        this->Bitangent = vec3(0, 0, 0);
    }
};

class Triangle {
public:
    //vec3 p1, p2, p3;   
    //vec3 n1, n2, n3;    
    std::vector<Vertex> vertex;
};

struct Triangle_encoded {
    vec3 p1, p2, p3;    
    vec3 n1, n2, n3;    
};


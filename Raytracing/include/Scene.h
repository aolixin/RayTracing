#pragma once

#include "Model.h"
#include "Triangle.h"
#include "Material.h"
#include "RenderNode.h"

using namespace std;

class Scene
{
public:
    GLuint trianglesTextureBuffer;
    GLuint nodesTextureBuffer;
    GLuint materialsTextureBuffer;

    int nTriangles = -1;
    int nNodes = -1;
    int nMaterials = -1;

public:
    vector<RenderNode>render_nodes;
    vector<Model> models;
    vector<Triangle> triangles;
    vector<Material> materials;
    BVH myBVH;

    void Add(Model model,Material material);

    void SetupScene();

    void Draw();

    void BuildDatas();
};

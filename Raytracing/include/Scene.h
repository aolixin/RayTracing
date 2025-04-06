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

    int materials_count = 0;

public:
    vector<RenderNode>render_nodes;
    // vector<Model> models;
    vector<Triangle> triangles_expand;
    vector<Material> materials_expand;
    BVH myBVH;

    void Add(Model model,Material material);

    void SetupGIScene();

    void Draw();

    void GenBuffers();
};

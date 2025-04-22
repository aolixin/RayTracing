#pragma once

#include "KDTree.h"
#include "Model.h"
#include "Triangle.h"
#include "Material.h"
#include "RenderNode.h"
#include "Octree.h"
// #include "KdTree.h"


using namespace std;

class Scene
{
public:
    GLuint trianglesTextureBuffer;
    GLuint nodesTextureBuffer;
    GLuint materialsTextureBuffer;

    GLuint envCubeMap;
    GLuint hdrMap;
    GLuint hdrCache;

    int hdrWidth = 0;
    int hdrHeight = 0;

    int nTriangles = -1;
    int nNodes = -1;
    int nMaterials = -1;

    int materials_count = 0;

public:
    vector<RenderNode>render_nodes;
    // vector<Model> models;
    vector<Triangle> triangles_expand;
    vector<Material> materials_expand;

#ifdef USE_BVH
    BVH myBVH;
#endif

#ifdef USE_OCTREE
    Octree myOctree;
#endif

#ifdef USE_KDTREE
    KDTree myKdTree;
#endif
    
    
    void Add(Model model,Material material,glm::mat4 modelMatrix = glm::mat4(1.0f));

    void SetupGIScene();

    void Draw();

    void GenBuffers();
};

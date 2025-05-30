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

	int nTriangles = 0;
	int nNodes = 0;
	int nMaterials = 0;

	int materials_count = 0;

public:
	vector<RenderNode>render_nodes;
	// vector<Model> models;
	vector<Triangle> triangles_expand;
	vector<Material> materials_expand;

	BVH myBVH;

	Octree myOctree;

	KDTree myKdTree;


	void Add(std::shared_ptr<Model>&model, Material material, glm::mat4 modelMatrix = glm::identity<glm::mat4>());

	void SetupGIScene();

	//void Draw();

	void GenBuffers();

	~Scene();
};

#include "Scene.h"
#include "GlobalFeat.h"
#include <chrono>

void Scene::Add(std::shared_ptr<Model>& model, Material material, glm::mat4 modelMatrix)
{
	// models.push_back(model);
	{
		render_nodes.emplace_back(
			model,
			make_shared<Material>(material),
			modelMatrix
		);
	}
}

void Scene::SetupGIScene()
{
	for (const auto& render_node : render_nodes)
	{
		int start = triangles_expand.size();
		for (auto& mesh : render_node.model->meshes)
			triangles_expand.insert(triangles_expand.end(), mesh->triangles.begin(),
				mesh->triangles.end());

		for (int i = start; i < triangles_expand.size(); i++)
		{
			triangles_expand[i].materialID = materials_count;
			// trans
			triangles_expand[i].vertex[0].Position = render_node.modelMatrix * vec4(
				triangles_expand[i].vertex[0].Position, 1.0f);
			triangles_expand[i].vertex[1].Position = render_node.modelMatrix * vec4(
				triangles_expand[i].vertex[1].Position, 1.0f);
			triangles_expand[i].vertex[2].Position = render_node.modelMatrix * vec4(
				triangles_expand[i].vertex[2].Position, 1.0f);
		}
		materials_expand.push_back(*render_node.material);
		materials_count++;
	}

	auto start = std::chrono::high_resolution_clock::now();

	if (renderPath == RenderPath::RT || renderPath == RenderPath::DebugBVH || renderPath == RenderPath::TestBVH)
	{
		this->myBVH.triangles = this->triangles_expand;
		this->myBVH.BuildBVH(0, this->triangles_expand.size() - 1);
		// this->myBVH.BuildBVHWithSAH(0, this->triangles_expand.size() - 1);
		if (renderPath == RenderPath::DebugBVH)
		{
			// this->myBVH.BuildDebugBVHTree(DEBUG_BVH_START_DEPTH,DEBUG_BVH_END_DEPTH);
			this->myBVH.BuildDebugBVHTree_l(DEBUG_BVH_START_DEPTH, DEBUG_BVH_END_DEPTH);
		}
		else if (renderPath == RenderPath::TestBVH) {
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();

			std::cout << "build time = " << duration << " ms" << std::endl;

			cout << "total memory: " << this->myBVH.GetMemoryUsageInKB() << " kb" << endl;
		}
	}

	else if (renderPath == RenderPath::DebugOctree || renderPath == RenderPath::TestOctree)
	{
		this->myOctree = Octree(this->triangles_expand);
		if (renderPath == RenderPath::DebugOctree)
		{
			this->myOctree.BuildDebugOctree_l(DEBUG_OCTREE_START_DEPTH, DEBUG_OCTREE_END_DEPTH);
		}
		else if (renderPath == RenderPath::TestOctree) {
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();
			std::cout << "build time = " << duration << " ms" << std::endl;
			cout << "total memory: " << this->myOctree.getMemoryUsageInKB() << " kb" << endl;
		}

	}
	else if (renderPath == RenderPath::DebugKdTree || renderPath == RenderPath::TestKdTree) {
		this->myKdTree = KDTree(this->triangles_expand);
		if (renderPath == RenderPath::DebugKdTree)
		{
			this->myKdTree.BuildDebugKdTree_l(DEBUG_KDTREE_START_DEPTH, DEBUG_KDTREE_END_DEPTH);
		}
		else if (renderPath == RenderPath::TestKdTree) {
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count();
			std::cout << "build time = " << duration << " ms" << std::endl;
			cout << "total memory: " << this->myKdTree.getMemoryUsageInKB() << " kb" << endl;
		}
	}

	// std::cout << "SAH BVHNode:    " << this->myBVH.nodes.size() << endl;

	if (renderPath == RenderPath::RT)
		GenBuffers();

}


void Scene::GenBuffers()
{

	vector<Triangle_encoded> triangles_encoded(this->myBVH.triangles.size());
	for (int i = 0; i < this->myBVH.triangles.size(); i++)
	{
		Triangle& t = this->myBVH.triangles[i];

		triangles_encoded[i].p1 = t.vertex[0].Position;
		triangles_encoded[i].p2 = t.vertex[1].Position;
		triangles_encoded[i].p3 = t.vertex[2].Position;

		triangles_encoded[i].n1 = t.vertex[0].Normal;
		triangles_encoded[i].n2 = t.vertex[1].Normal;
		triangles_encoded[i].n3 = t.vertex[2].Normal;

		triangles_encoded[i].materialID = vec3(t.materialID, 0, 0);
	}

	vector<BVHNode_encoded> nodes_encoded(this->myBVH.nodes.size());
	for (int i = 0; i < this->myBVH.nodes.size(); i++)
	{
		nodes_encoded[i].childs = vec3(this->myBVH.nodes[i].left, this->myBVH.nodes[i].right, 0);
		nodes_encoded[i].leafInfo = vec3(this->myBVH.nodes[i].n, this->myBVH.nodes[i].index, 0);
		nodes_encoded[i].AA = this->myBVH.nodes[i].AA;
		nodes_encoded[i].BB = this->myBVH.nodes[i].BB;
	}


	vector<Material_encoded> materials_encoded(this->materials_expand.size());
	for (int i = 0; i < materials_encoded.size(); i++)
	{
		materials_encoded[i] = materials_expand[i].encoded();
	}

	GLuint tbo0;
	glGenBuffers(1, &tbo0);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
	glBufferData(GL_TEXTURE_BUFFER, triangles_encoded.size() * sizeof(Triangle_encoded), &triangles_encoded[0],
		GL_STATIC_DRAW);
	glGenTextures(1, &trianglesTextureBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

	nTriangles = triangles_encoded.size();
	triangles_encoded.clear();

	GLuint tbo1;
	glGenBuffers(1, &tbo1);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
	glBufferData(GL_TEXTURE_BUFFER, nodes_encoded.size() * sizeof(BVHNode_encoded), &nodes_encoded[0],
		GL_STATIC_DRAW);
	glGenTextures(1, &nodesTextureBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);

	nNodes = nodes_encoded.size();
	nodes_encoded.clear();

	GLuint tbo2;
	glGenBuffers(1, &tbo2);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo2);
	glBufferData(GL_TEXTURE_BUFFER, materials_encoded.size() * sizeof(Material_encoded), &materials_encoded[0],
		GL_STATIC_DRAW);
	glGenTextures(1, &materialsTextureBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, materialsTextureBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo2);

	nMaterials = materials_encoded.size();
	materials_encoded.clear();

}


Scene::~Scene() {
	glDeleteTextures(1, &trianglesTextureBuffer);
	glDeleteTextures(1, &nodesTextureBuffer);
	glDeleteTextures(1, &materialsTextureBuffer);
	glDeleteBuffers(1, &trianglesTextureBuffer);
	glDeleteBuffers(1, &nodesTextureBuffer);
	glDeleteBuffers(1, &materialsTextureBuffer);
	hdrWidth = 0;
	hdrHeight = 0;

	nTriangles = 0;
	nNodes = 0;
	nMaterials = 0;

	materials_count = 0;

	render_nodes.clear();
	triangles_expand.clear();
	materials_expand.clear();




	cout << "Scene destructor" << endl;
}
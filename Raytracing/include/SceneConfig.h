#pragma once
#include "Utils.h"
#include "Scene.h"
#include <memory.h>
#include <vector>
#include <functional>



std::shared_ptr<Scene> BuildRoom();
std::shared_ptr<Scene> BuildRoomAndBox();
std::shared_ptr<Scene> BuildRoomAndBunny();

// Debug
std::shared_ptr<Scene> BuildPlaneAndBunny();
std::shared_ptr<Scene> BuildSphere_simple();
std::shared_ptr<Scene> BuildSphere_complex();

vector<function<std::shared_ptr<Scene>()>>sceneList = {
	BuildRoom,
	BuildRoomAndBox,
	BuildRoomAndBunny,
	
	BuildPlaneAndBunny,
	BuildSphere_simple,
	BuildSphere_complex
};


std::shared_ptr<Scene> BuildScene(int idx = 0)
{
	if (idx >= sceneList.size())
	{
		cout << "Error: Scene index out of range!" << endl;
		return nullptr;
	}
	return sceneList[idx]();

	//if (renderPath == RenderPath::Forward || renderPath == RenderPath::RT)
	//{
	//	return BuildRoomAndBunny();
	//}
	//else if (renderPath == RenderPath::DebugIA)
	//{
	//	return BuildRoomAndBunny();
	//}
	//else if (renderPath == RenderPath::DebugBVH ||
	//	renderPath == RenderPath::DebugOctree ||
	//	renderPath == RenderPath::DebugKdTree)
	//{
	//	return BuildPlaneAndBunny();
	//}
	//else if (renderPath == RenderPath::TestBVH ||
	//	renderPath == RenderPath::TestOctree ||
	//	renderPath == RenderPath::TestKdTree)
	//{
	//	return BuildSphere_complex();
	//}
	//else
	//{
	//	cout << "Error: Invalid render path!" << endl;
	//	return nullptr;
	//}
}


std::shared_ptr<Scene> BuildRoom()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// bunny
	Shader phongShader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");



	// left
	Material mat1(phongShader);
	mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
	mat1.specular = 0.0f;
	std::shared_ptr<Model> left = make_shared<Model>("Resources/models/cornellbox/left.obj");

	// right
	Material mat2(phongShader);
	mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
	mat2.specular = 0.0f;
	std::shared_ptr<Model> right = make_shared<Model>("Resources/models/cornellbox/right.obj");

	// floor
	Material mat3(phongShader);
	mat3.baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	mat3.roughness = 0.5f;
	mat3.metallic = 0.0f;
	std::shared_ptr<Model> floor = make_shared<Model>("Resources/models/cornellbox/floor.obj");

	// light
	Material mat4(phongShader);
	mat4.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	mat4.emissive = glm::vec3(1.0f, 1.0f, 1.0f);
	std::shared_ptr<Model> light = make_shared<Model>("Resources/models/cornellbox/light.obj");

	// short


	myScene->Add(left, mat1);
	myScene->Add(right, mat2);
	myScene->Add(floor, mat3);
	myScene->Add(light, mat4);


	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;
}

std::shared_ptr<Scene> BuildRoomAndBunny()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// bunny
	Shader phongShader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");

	Material mat0(phongShader);
	mat0.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
	mat0.specular = 0.0f;
	std::shared_ptr<Model> bunny = make_shared<Model>("Resources/models/bunny.obj");

	// left
	Material mat1(phongShader);
	mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
	mat1.specular = 0.0f;
	std::shared_ptr<Model> left = make_shared<Model>("Resources/models/cornellbox/left.obj");

	// right
	Material mat2(phongShader);
	mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
	mat2.specular = 0.0f;
	std::shared_ptr<Model> right = make_shared<Model>("Resources/models/cornellbox/right.obj");

	// floor
	Material mat3(phongShader);
	mat3.baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	mat3.roughness = 0.5f;
	mat3.metallic = 0.0f;
	std::shared_ptr<Model> floor = make_shared<Model>("Resources/models/cornellbox/floor.obj");

	// light
	Material mat4(phongShader);
	mat4.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	mat4.emissive = glm::vec3(1.0f, 1.0f, 1.0f);
	std::shared_ptr<Model> light = make_shared<Model>("Resources/models/cornellbox/light.obj");

	myScene->Add(bunny, mat0);
	myScene->Add(left, mat1);
	myScene->Add(right, mat2);
	myScene->Add(floor, mat3);
	myScene->Add(light, mat4);


	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;
}

std::shared_ptr<Scene> BuildRoomAndBox()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// bunny
	Shader phongShader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");

	// left
	Material mat1(phongShader);
	mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
	mat1.specular = 0.0f;
	std::shared_ptr<Model> left = make_shared<Model>("Resources/models/cornellbox/left.obj");

	// right
	Material mat2(phongShader);
	mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
	mat2.specular = 0.0f;
	std::shared_ptr<Model> right = make_shared<Model>("Resources/models/cornellbox/right.obj");

	// floor
	Material mat3(phongShader);
	mat3.baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	mat3.roughness = 0.5f;
	mat3.metallic = 0.0f;
	std::shared_ptr<Model> floor = make_shared<Model>("Resources/models/cornellbox/floor.obj");

	// light
	Material mat4(phongShader);
	mat4.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	mat4.emissive = glm::vec3(1.0f, 1.0f, 1.0f);
	std::shared_ptr<Model> light = make_shared<Model>("Resources/models/cornellbox/light.obj");

	// short
	Material mat5(phongShader);
	mat5.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
	mat5.specular = 0.0f;
	std::shared_ptr<Model> shortBox = make_shared<Model>("Resources/models/cornellbox/short.obj");


	// tall
	Material mat6(phongShader);
	mat6.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
	mat6.specular = 0.0f;
	std::shared_ptr<Model> tallBox = make_shared<Model>("Resources/models/cornellbox/tall.obj");

	myScene->Add(left, mat1);
	myScene->Add(right, mat2);
	myScene->Add(floor, mat3);
	myScene->Add(light, mat4);
	myScene->Add(shortBox, mat5);
	myScene->Add(tallBox, mat6);


	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;
}


std::shared_ptr<Scene> BuildPlaneAndBunny()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// bunny
	Shader shader0("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");
	Material mat0(shader0);
	mat0.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
	mat0.specular = 0.0f;
	std::shared_ptr<Model> model0 = make_shared<Model>("Resources/models/bunny.obj");
	myScene->Add(model0, mat0);

	// plane
	Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");
	Material mat1(shader1);
	mat1.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
	mat1.metallic = 1.0f;
	mat1.specular = 1.0f;
	std::shared_ptr<Model> model1 = make_shared<Model>("Resources/models/plane.obj");
	myScene->Add(model1, mat1);

	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;
}



std::shared_ptr<Scene> BuildSphere_simple()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// spheres
	Shader shader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
	std::shared_ptr<Model> sphere = make_shared<Model>("Resources/models/sphere_simple.obj");

	// First sphere
	Material m(shader);

	// Second sphere
	m.baseColor = vec3(1, 1, 1);
	m.metallic = 0.9;
	m.roughness = 0.2;
	glm::mat4 trans8 = glm::translate(identity, glm::vec3(0.0f, -0.6f, 0.0f));
	myScene->Add(sphere, m, trans8);


	// plane
	Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
	Material mat1(shader1);
	mat1.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	mat1.specular = 0.9f;
	mat1.roughness = 0.4f;
	std::shared_ptr<Model> model1 = make_shared<Model>("Resources/models/plane.obj");
	myScene->Add(model1, mat1);


	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;

	return myScene;
}

std::shared_ptr<Scene> BuildSphere_complex()
{
	std::shared_ptr<Scene> myScene = make_shared<Scene>();
	glm::mat4 identity = glm::identity<glm::mat4>();

	// spheres
	Shader shader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
	std::shared_ptr<Model> sphere = make_shared<Model>("Resources/models/sphere.obj");

	// First sphere
	Material m(shader);
	m.baseColor = vec3(1, 1, 1);

	float xOffset = 1.0f;

	glm::mat4 trans = glm::translate(identity, glm::vec3(0.0f, -0.6f, 0.0f));
	myScene->Add(sphere, m, trans);
	trans = glm::translate(identity, glm::vec3(xOffset, -0.6f, 0.0f));
	myScene->Add(sphere, m, trans);
	trans = glm::translate(identity, glm::vec3(-xOffset, -0.6f, 0.0f));
	myScene->Add(sphere, m, trans);

	float yOffset = 0.6;
	trans = glm::translate(identity, glm::vec3(0.0f, yOffset, 0.0f));
	myScene->Add(sphere, m, trans);
	trans = glm::translate(identity, glm::vec3(xOffset, yOffset, 0.0f));
	myScene->Add(sphere, m, trans);
	trans = glm::translate(identity, glm::vec3(-xOffset, yOffset, 0.0f));
	myScene->Add(sphere, m, trans);


	// plane
	Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
	Material mat1(shader1);
	mat1.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	std::shared_ptr<Model> model1 = make_shared<Model>("Resources/models/plane.obj");
	myScene->Add(model1, mat1);

	float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
	myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

	myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

	if (data)
		free(data);

	return myScene;

	return myScene;
}

#pragma once
#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"
#include "Scene.h"
#include "Utils.h"
#include "RenderContext.h"
#include "GlobalFeat.h"


class Renderer
{
private:
	Shader RTShader;
	Shader screenShader;
	Shader postShader;
	Shader skyboxShader;
	Shader unlitShader;

	Shader debug_ia_shader;
	Shader testShader;


	GLuint frameBuffer0;
	vector<GLuint> frameTextures0;

	GLuint frameBuffer1;
	vector<GLuint> frameTextures1;

	GLuint TestVAO, TestVBO;



	int frameCount = 0;
	static bool already_init;

public:
	Renderer()
	{
	}

	//RenderPath renderPath = RenderPath::Forward;
	static std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Scene> scene;

	static GLFWwindow* window;
	bool firstMouse = true;
	float lastX = RENDER_WIDTH / 2.0f;
	float lastY = RENDER_HEIGHT / 2.0f;


public:
	static std::shared_ptr<Renderer> GetRenderer(RenderPath path = RenderPath::Forward, GLFWwindow* window = nullptr);

	void InitRenderer();

	void ResetRender(/*std::shared_ptr<Camera> newCamera, std::shared_ptr<Scene> newScene*/);

	void SetupScene(std::shared_ptr<Scene> scene);

	void Draw(GLuint targetFrameBuffer = 0);

	void FrameBufferToScreen(GLuint frameBufferTexture);

	static void DestroyRenderer();

	void RegisterCallback();

	int RendererClose();

	void processInput(float deltaTime);

	void DrawSkybox();


	glm::mat4 Perspective();

	glm::mat4 View();

	glm::mat4 CameraRotate();

	void SwapBuffers();

	void PollEvents();

	void Terminate();



private:

	int debugDepth = 0;
	struct Ray;

	struct HitResult;

	BVHNode GetBVHNode(int i);

	HitResult HitTriangle(Triangle triangle, Ray ray);

	Triangle GetTriangle(int i);

	HitResult HitArray(Ray ray, int l, int r);

	float HitAABB(Ray r, vec3 AA, vec3 BB);

	HitResult HitBVH(Ray ray);

	HitResult HitOctree(Ray ray);

	HitResult HitKDTree(Ray ray);


public:
	void TestDraw(GLuint targetFrameBuffer = 0);

	void DrawFramwBuffer(GLuint targetFrameBuffer = 0);


};

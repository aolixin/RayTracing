#if 1

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include "GlobalFeat.h"
#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneConfig.h"




void MainSingleFrame(shared_ptr<Renderer> renderer, float deltaTime) {
	renderer->processInput(deltaTime);

#ifdef TEST_MODE
	renderer->DrawFramwBuffer();
#elif defined(DEBUG_MODE)
	renderer->Draw();
#else
	renderer->Draw();
#endif
	renderer->SwapBuffers();
	renderer->PollEvents();
}


int main()
{
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);

	shared_ptr<Scene> myScene = BuildScene();

	renderer->SetupScene(myScene);

#ifdef TEST_MODE
	shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 7.0f));
#elif defined(DEBUG_MODE)
	shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(-3.0f, 1.2f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -55.0f,
		-15.0f);
#else
	shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
	
#endif

	renderer->camera = camera;

#ifdef TEST_MODE
	renderer->TestDraw();
#endif

	int frameCount = 0;
	while (!renderer->RendererClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		MainSingleFrame(renderer, deltaTime);
	}

	renderer->Terminate();
	return 0;
}



#endif

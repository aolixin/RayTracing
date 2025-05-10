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
	// game
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frameCount = 0;

	const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);
	shared_ptr<Scene> myScene = BuildScene();

	vector<GLuint> gameFrameBufferTextures;
	GLuint gameFrameBuffer = GetFrameBuffer(RENDER_WIDTH, RENDER_HEIGHT, gameFrameBufferTextures, 1, 1);

	GLuint ldr;


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
	renderer->TestDraw(gameFrameBuffer);
#endif


	// imgui
	// Setup Dear ImGui context
	GLFWwindow* editorWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, renderer->window);
	IMGUI_CHECKVERSION();
	glfwMakeContextCurrent(editorWindow);
	glfwSwapInterval(1); // Enable vsync
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(editorWindow, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);


	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	while (!renderer->RendererClose())
	{
		// game
		glfwMakeContextCurrent(renderer->window);
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		renderer->processInput(deltaTime);

#ifdef TEST_MODE
		renderer->DrawFramwBuffer(gameFrameBuffer);
#elif defined(DEBUG_MODE)
		renderer->Draw(gameFrameBuffer);
#else
		renderer->Draw(gameFrameBuffer);
#endif

		renderer->FrameBufferToScreen(gameFrameBufferTextures[0]);


		//renderer->SwapBuffers();
		//renderer->PollEvents();


		glfwMakeContextCurrent(editorWindow);
		glfwSwapInterval(1);
		if (glfwGetWindowAttrib(editorWindow, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}


		// editor

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{

			ImGui::Begin("game");
			ImTextureID image_id = (ImTextureID)(intptr_t)gameFrameBufferTextures[0];
			ImGui::Text("pointer = %x", image_id);
			ImGui::Text("size = %d x %d", RENDER_WIDTH, RENDER_HEIGHT);

			ImGui::Image(image_id, ImVec2(RENDER_WIDTH, RENDER_HEIGHT), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0), ImVec4(1, 1, 1, 1), ImVec4(0, 1, 0, 1));
			
			//ImGui::Image(image_id, ImVec2(RENDER_WIDTH, RENDER_HEIGHT));
			ImGui::End();

			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is some useful text.");
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);

			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}


		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(editorWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		//MainSingleFrame(renderer, deltaTime);
		glfwSwapBuffers(editorWindow);
		glfwPollEvents();

	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	renderer->Terminate();
	return 0;
}



#endif

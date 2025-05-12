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




int main()
{
	// game
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frameCount = 0;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);


	// game
	GLFWwindow* gameWindow = glfwCreateWindow(RENDER_WIDTH, RENDER_HEIGHT, "RT", NULL, NULL);
	if (gameWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(gameWindow);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}

	// renderer

	const shared_ptr<Renderer> renderer = Renderer::GetRenderer(renderPath, gameWindow);
	shared_ptr<Scene> myScene = BuildScene();
	renderer->SetupScene(myScene);

	vector<GLuint> gameFrameBufferTextures;
	GLuint gameFrameBuffer = GetFrameBuffer(RENDER_WIDTH, RENDER_HEIGHT, gameFrameBufferTextures, 1, 1);

	shared_ptr<Camera> camera;
	if (renderPath <= DebugIA)
	{
		camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
	}
	else if (renderPath <= DebugKdTree)
	{
		camera = make_shared<Camera>(glm::vec3(-3.0f, 1.2f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -55.0f, -15.0f);
	}
	else if (renderPath <= TestKdTree)
	{
		camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 7.0f));
	}


	renderer->camera = camera;

	if (renderPath >= RenderPath::TestBVH && renderPath <= RenderPath::TestKdTree)
	{
		renderer->TestDraw(gameFrameBuffer);
	}


	// imgui
	// Setup Dear ImGui context
	GLFWwindow* editorWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, gameWindow);
	IMGUI_CHECKVERSION();
	glfwMakeContextCurrent(editorWindow);
	glfwSwapInterval(1); // Enable vsync
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(editorWindow, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);


	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	auto changeRenderPath = [&](RenderPath path) {
		glfwMakeContextCurrent(gameWindow);
		renderPath = path;
		renderer->ResetRender();
		myScene = BuildScene();
		renderer->SetupScene(myScene);

		if (renderPath <= DebugIA) {
			camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
		}
		else if (renderPath <= DebugKdTree) {
			camera = make_shared<Camera>(glm::vec3(-3.0f, 1.2f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -55.0f, -15.0f);
		}
		else if (renderPath <= TestKdTree) {
			camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 7.0f));
		}

		renderer->camera = camera;
		if (renderPath >= RenderPath::TestBVH && renderPath <= RenderPath::TestKdTree)
		{
			renderer->TestDraw(gameFrameBuffer);
		}
		glfwMakeContextCurrent(editorWindow);
		};

	while (!renderer->RendererClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// game
		glfwMakeContextCurrent(gameWindow);
		//glfwHideWindow(renderer->window);


		renderer->processInput(deltaTime);

		if (renderPath <= RenderPath::DebugKdTree) {
			renderer->Draw(gameFrameBuffer);
		}
		else {
			renderer->DrawFramwBuffer(gameFrameBuffer);
		}


		//renderer->FrameBufferToScreen(gameFrameBufferTextures[0]);

		//renderer->SwapBuffers();
		//renderer->PollEvents();

		// editor
		glfwMakeContextCurrent(editorWindow);
		
		//glfwShowWindow(editorWindow);
		glfwSwapInterval(1);
		if (glfwGetWindowAttrib(editorWindow, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}



		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{

			ImGui::Begin("game");
			ImTextureID image_id = (ImTextureID)(intptr_t)gameFrameBufferTextures[0];
			ImGui::Text("size = %d x %d", RENDER_WIDTH, RENDER_HEIGHT);
			ImGui::Image(image_id, ImVec2(RENDER_WIDTH, RENDER_HEIGHT), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0), ImVec4(1, 1, 1, 1), ImVec4(0, 1, 0, 1));
			ImGui::End();




			ImGui::Begin("editor");

			static int pathIdx = renderPath;
			if (ImGui::Combo("Mode##Selector", &pathIdx, "RT\0Forward\0DebugIA\0DebugBVH\0DebugOctree\0DebugKdTree\0TestBVH\0TestOctree\0TestKdTree\0"))
			{
				changeRenderPath(RenderPath(pathIdx));
			}

			{
				ImGui::Separator();
				ImGui::Text("Test Metrics:");
				ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
				ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
			}

			if (renderPath >= RenderPath::TestBVH && renderPath <= RenderPath::TestKdTree)
			{
				ImGui::Separator();
				// time
				ImGui::Text("Build Time: %.3f ms", test_build_time);

				// memory
				ImGui::Text("Memory Usage:");
				ImGui::Text("Total Memory: %.2f MB", test_total_memory / (1024.0f * 1024.0f));
				ImGui::Text("Node Memory: %.2f MB", test_node_memory / (1024.0f * 1024.0f));
				ImGui::Text("Triangle Memory: %.2f MB", test_tri_memory / (1024.0f * 1024.0f));
				
				// nodes
				ImGui::Separator();
				ImGui::Text("Structure Info:");
				ImGui::Text("Node Count: %d", test_node_count);
				ImGui::Text("Triangle Count: %d", test_tri_count);
				ImGui::Text("Average Memory per Node: %.2f KB", test_node_memory / (1024.0f * test_node_count));
				ImGui::Text("Average Memory per Triangle: %.2f KB", test_tri_memory / (1024.0f * test_tri_count));
			}

			ImGui::End();
		}

		ImGui::Render();
		glfwSwapBuffers(editorWindow);
		glfwPollEvents();
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	renderer->Terminate();
	return 0;
}



#endif

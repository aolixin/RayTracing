#pragma once


#include "Renderer.h"


bool Renderer::already_init = false;
std::shared_ptr<Renderer> Renderer::renderer = nullptr;
GLFWwindow* Renderer::window = nullptr;

std::shared_ptr<Renderer> Renderer::GetRenderer(RenderPath path)
{
    if (renderer == nullptr)
    {
        renderer = std::make_shared<Renderer>();
        renderer->renderPath = path;
        renderer->InitRenderer();
    }
    return renderer;
}

void Renderer::InitRenderer()
{
    if (already_init)return;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    RegisterCallback();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // init shader
    RTShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/DebugBVH.frag");
    screenShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/framebuffers_screen.frag");

    frameBuffer0 = GetFrameBuffer(SCR_WIDTH, SCR_HEIGHT, frameTextures, 1, 1);
}

void Renderer::DestroyRenderer()
{
}

void Renderer::SetupScene(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    if (renderPath == RenderPath::GI)
    {
        this->scene->SetupGIScene();
    }
}

void Renderer::Draw()
{
    RenderContext context;
    context.projection = Perspective();
    context.view = View();
    context.viewPos = camera->Position;

    if (renderPath == RenderPath::Forward)
    {
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (auto& render_node : scene->render_nodes)
        {
            render_node.Draw(context);
        }
    }
    else if (renderPath == RenderPath::GI)
    {
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
        // glEnable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.3f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RTShader.use();
        RTShader.setInt("FrameCounter", frameCount++);
        RTShader.setInt("Width", SCR_WIDTH);
        RTShader.setInt("Height", SCR_HEIGHT);
        RTShader.setVec3("CameraPos", camera->Position);
        RTShader.setMat4("CameraRotate", inverse(CameraRotate()));  

        RTShader.setTextureBuffer("triangles", scene->trianglesTextureBuffer, 2);
        RTShader.setInt("nTriangles", scene->nTriangles);

        RTShader.setTextureBuffer("nodes", scene->nodesTextureBuffer, 3);
        RTShader.setInt("nNodes", scene->nNodes);

        RTShader.setTextureBuffer("materials", scene->materialsTextureBuffer, 4);
        RTShader.setInt("nMaterials", scene->nMaterials);

        
        
        DrawQuad(RTShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.3f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 

        screenShader.use();
        screenShader.setTexture("screenTexture", frameTextures[0], 6);

        DrawQuad(screenShader);
    }
}

glm::mat4 Renderer::Perspective()
{
    if (!camera)return glm::mat4(1.0f);
    return glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

glm::mat4 Renderer::View()
{
    return camera->GetViewMatrix();
}

glm::mat4 Renderer::CameraRotate()
{
    return camera->GetRotateMatrix();
}

int Renderer::RendererClose()
{
    return glfwWindowShouldClose(window);
}


void Renderer::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void Renderer::PollEvents()
{
    glfwPollEvents();
}

void Renderer::Terminate()
{
    glfwTerminate();
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (Renderer::GetRenderer()->firstMouse)
    {
        Renderer::GetRenderer()->lastX = xpos;
        Renderer::GetRenderer()->lastY = ypos;
        Renderer::GetRenderer()->firstMouse = false;
    }

    float xoffset = xpos - Renderer::GetRenderer()->lastX;
    float yoffset = Renderer::GetRenderer()->lastY - ypos;

    Renderer::GetRenderer()->lastX = xpos;
    Renderer::GetRenderer()->lastY = ypos;

    Renderer::GetRenderer()->camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Renderer::GetRenderer()->camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::RegisterCallback()
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void Renderer::processInput(float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

void Renderer::DrawSkybox(Shader passToScreenShader, GLint envCubemap)
{
    glDepthFunc(GL_LEQUAL);
    passToScreenShader.use();
    glm::mat4 passToProjection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                  0.1f, 100.0f);
    glm::mat4 passToView = camera->GetViewMatrix();

    passToScreenShader.setMat4("projection", passToProjection);
    passToScreenShader.setMat4("view", passToView);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    passToScreenShader.setInt("environmentMap", 0);


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    DrawCube(passToScreenShader);

    glDepthFunc(GL_LESS);
}

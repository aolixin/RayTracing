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

    if (renderPath != RenderPath::GI)
    {
        RegisterCallback();
    }


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    glEnable(GL_DEPTH_TEST);

    // init shader
    RTShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/RT.frag");
    screenShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/framebuffersToscreen.frag");
    postShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/post.frag");
    skyboxShader = Shader("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
    unlitShader = Shader("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");

    debug_ia_shader = Shader("Resources/shaders/screen.vert", "Resources/shaders/debug_ia.frag");

    frameBuffer0 = GetFrameBuffer(SCR_WIDTH, SCR_HEIGHT, frameTextures0, 1, 0);

    frameBuffer1 = GetFrameBuffer(SCR_WIDTH, SCR_HEIGHT, frameTextures1, 1, 0);
}

void Renderer::DestroyRenderer()
{
}

void Renderer::SetupScene(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    if (renderPath == RenderPath::GI || renderPath == RenderPath::DebugBVH)
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
        // glEnable(GL_CULL_FACE);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (auto& render_node : scene->render_nodes)
        {
            render_node.Draw(context);
        }
        DrawSkybox();
    }
    else if (renderPath == RenderPath::GI)
    {
        // pass1
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
        // glEnable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RTShader.use();
        RTShader.setInt("frameCounter", frameCount++);
        RTShader.setInt("width", SCR_WIDTH);
        RTShader.setInt("height", SCR_HEIGHT);
        RTShader.setVec3("cameraPos", camera->Position);
        RTShader.setMat4("cameraRotate", inverse(CameraRotate()));

        RTShader.setTextureBuffer("triangles", scene->trianglesTextureBuffer, 2);
        RTShader.setInt("nTriangles", scene->nTriangles);

        RTShader.setTextureBuffer("nodes", scene->nodesTextureBuffer, 3);
        RTShader.setInt("nNodes", scene->nNodes);

        RTShader.setTextureBuffer("materials", scene->materialsTextureBuffer, 4);
        RTShader.setInt("nMaterials", scene->nMaterials);

        RTShader.setTexture("lastFrame", frameTextures1[0], 5);

        RTShader.setTexture("hdrMap", scene->hdrMap, 6);
        RTShader.setTexture("hdrCache", scene->hdrCache, 7);
        RTShader.setInt("hdrResolution", scene->hdrWidth);

        DrawQuad(RTShader);


        // pass2
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.use();
        screenShader.setTexture("screenTexture", frameTextures0[0], 6);
        DrawQuad(screenShader);


        // pass3
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postShader.use();
        postShader.setTexture("screenTexture", frameTextures1[0], 6);
        DrawQuad(postShader);
    }
    else if (renderPath == RenderPath::DebugIA)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        debug_ia_shader.use();

        debug_ia_shader.setInt("frameCounter", frameCount++);
        debug_ia_shader.setInt("width", SCR_WIDTH);
        debug_ia_shader.setInt("height", SCR_HEIGHT);

        debug_ia_shader.setTexture("hdrMap", scene->hdrMap, 2);
        debug_ia_shader.setTexture("hdrCache", scene->hdrCache, 3);
        debug_ia_shader.setInt("hdrResolution", scene->hdrWidth);

        DrawQuad(postShader);
    }
    else if (renderPath == RenderPath::DebugBVH)
    {
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        // glEnable(GL_CULL_FACE);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // draw meshs
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (auto& render_node : scene->render_nodes)
        {
            render_node.Draw(context);
        }

        // draw bvh
        unlitShader.use();
        unlitShader.setMat4("projection", context.projection);
        unlitShader.setMat4("view", context.view);
        unlitShader.setMat4("model", glm::mat4(1.0f));
        unlitShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));

        // glBindVertexArray(scene->myBVH.DebugVAO);
        // glDrawElements(GL_TRIANGLES, scene->myBVH.DebugIndices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        glLineWidth(2.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBindVertexArray(scene->myBVH.DebugVAO);
        glDrawElements(GL_LINES, scene->myBVH.DebugIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // DrawSkybox();
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

    if (renderPath == RenderPath::GI)
    {
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

void Renderer::DrawSkybox()
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    glm::mat4 passToProjection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                  0.1f, 100.0f);
    glm::mat4 passToView = camera->GetViewMatrix();

    skyboxShader.setMat4("projection", passToProjection);
    skyboxShader.setMat4("view", passToView);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene->envCubeMap);
    skyboxShader.setInt("environmentMap", 0);


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    DrawCube(skyboxShader);

    glDepthFunc(GL_LESS);
}

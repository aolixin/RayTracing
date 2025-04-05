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

enum RenderPath
{
    Forward,
    GI
};




class Renderer
{
private:
    RenderPath renderPath = RenderPath::Forward;
public:
    Renderer()
    {
    }

    static std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;

    static constexpr unsigned int SCR_WIDTH = 800;
    static constexpr unsigned int SCR_HEIGHT = 600;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;

    bool firstMouse = true;

    static GLFWwindow* window;
    static bool already_init;

public:
    static std::shared_ptr<Renderer> GetRenderer();

    void InitRenderer();

    void SetupScene(std::shared_ptr<Scene> scene, RenderPath path = RenderPath::Forward);

    void Draw();

    static void DestroyRenderer();

    void RegisterCallback();

    int RendererClose();

    void processInput(float deltaTime);

    void DrawSkybox(Shader passToScreenShader, GLint envCubemap);


    glm::mat4 Perspective();

    glm::mat4 View();

    void SwapBuffers();

    void PollEvents();

    void Terminate();
};

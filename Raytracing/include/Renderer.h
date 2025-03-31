#pragma once
#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "Camera.h"

class Renderer
{
public:
    Renderer()
    {
    }

    static std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Camera> camera;

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

    static void DestroyRenderer();

    void RegisterCallback();

    int RendererClose();

    void processInput(float deltaTime);

    glm::mat4 Perspective();

    glm::mat4 View();

    void SwapBuffers();

    void PollEvents();

    void Terminate();
};

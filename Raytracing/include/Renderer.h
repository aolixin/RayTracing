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

    GLuint TestVAO,TestVBO;

public:
    Renderer()
    {
    }

    RenderPath renderPath = RenderPath::Forward;
    static std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;

    static constexpr unsigned int SCR_WIDTH = 1080;
    static constexpr unsigned int SCR_HEIGHT = 720;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;

    bool firstMouse = true;
    int frameCount = 0;

    static GLFWwindow* window;
    static bool already_init;

public:
    static std::shared_ptr<Renderer> GetRenderer(RenderPath path = RenderPath::Forward);

    void InitRenderer();

    void SetupScene(std::shared_ptr<Scene> scene);

    void Draw();

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

#ifdef TEST_ACCELERATION_STRUCTURE
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
    
    void TestDraw();

    void DrawFramwBuffer();

#endif
};

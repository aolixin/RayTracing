﻿#if 0

#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Utils.h"
#include "Scene.h"

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer();

    
    
    Shader phongShader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    // Shader DebugBVHShader("Resources/shaders/DebugBVH.vert", "Resources/shaders/DebugBVH.frag");
    Material phongMaterial(phongShader);

    
    Shader pass2SrcShader("Resources/shaders/passToScreen.vert", "Resources/shaders/passToScreen.frag");

    GLint envCubeMap =  buildEnvCubMap();
    
    // Model ourModel("Resources/models/backpack/backpack.obj");
    Model ourModel("Resources/models/bunny.obj");
    shared_ptr<Scene>myScene = make_shared<Scene>();
    myScene->Add(ourModel,phongMaterial);

    renderer->SetupScene(myScene);

    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    
    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));

    renderer->camera = camera;

    while (!renderer->RendererClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        renderer->processInput(deltaTime);

        renderer->Draw();
        
        // renderer->DrawSkybox(pass2SrcShader,envCubeMap);

        renderer->SwapBuffers();
        renderer->PollEvents();
    }

    renderer->Terminate();
    return 0;
}


#endif

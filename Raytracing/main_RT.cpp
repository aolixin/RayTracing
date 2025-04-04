#if 1

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
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RenderPath::GI);
    
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
    
    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 4.0f));

    renderer->camera = camera;

    int frameCount = 0;
    while (!renderer->RendererClose())
    {
        cout<<"Frame: " << frameCount++ << endl;
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

#if 1

#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneConfig.h"
#include "GlobalFeat.h"

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);

    shared_ptr<Scene> myScene = BuildScene();

    renderer->SetupScene(myScene);

    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 4.0f));

    renderer->camera = camera;

    int frameCount = 0;
    while (!renderer->RendererClose())
    {
        if(renderer->renderPath == RenderPath::GI || renderer->renderPath == RenderPath::DebugOctree)
        {
            cout << "Frame: " << frameCount++ << endl;
        }
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        renderer->processInput(deltaTime);

        renderer->Draw();
        
        renderer->SwapBuffers();
        renderer->PollEvents();
        // return 0;
    }

    renderer->Terminate();
    return 0;
}


#endif

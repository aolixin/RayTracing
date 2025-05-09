#if 0

#include "GlobalFeat.h"
#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneConfig.h"


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


#ifdef TEST_MODE
int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);

    shared_ptr<Scene> myScene = BuildScene();

    renderer->SetupScene(myScene);

    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 7.0f));

    renderer->camera = camera;

    renderer->TestDraw();

    int frameCount = 0;
    while (!renderer->RendererClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        renderer->processInput(deltaTime);

        renderer->DrawFramwBuffer();


        renderer->SwapBuffers();
        renderer->PollEvents();
        // return 0;
    }

    renderer->Terminate();
    return 0;
}

#elif defined(DEBUG_MODE)

int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);

    shared_ptr<Scene> myScene = BuildScene();

    renderer->SetupScene(myScene);

    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(-3.0f, 1.2f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -55.0f,
                                                   -15.0f);

    renderer->camera = camera;

    int frameCount = 0;
    while (!renderer->RendererClose())
    {
        cout << "Frame: " << frameCount++ << endl;

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

#elif defined(RELEASE_MODE)

int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer(RENDER_PATH);

    shared_ptr<Scene> myScene = BuildScene();

    renderer->SetupScene(myScene);

    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

    renderer->camera = camera;

    int frameCount = 0;
    while (!renderer->RendererClose())
    {
        if(renderer->renderPath == RenderPath::GI)
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

#endif
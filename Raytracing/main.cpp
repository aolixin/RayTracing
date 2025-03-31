#if 1

#include "Shader.h"
#include "Model.h"
#include "Renderer.h"

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    const shared_ptr<Renderer> renderer = Renderer::GetRenderer();

    Shader fwdShader("Resources/shaders/forward.vert", "Resources/shaders/forward.frag");

    Model ourModel("Resources/models/backpack/backpack.obj");
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

    shared_ptr<Camera> camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

    renderer->camera = camera;

    while (!renderer->RendererClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        renderer->processInput(deltaTime);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fwdShader.use();

        glm::mat4 projection = renderer->Perspective();
        glm::mat4 view = renderer->View();
        fwdShader.setMat4("projection", projection);
        fwdShader.setMat4("view", view);


        fwdShader.setMat4("model", model);
        ourModel.Draw(fwdShader);


        renderer->SwapBuffers();
        renderer->PollEvents();
    }

    renderer->Terminate();
    return 0;
}


#endif

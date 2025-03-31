#include "Renderer.h"

bool Renderer::already_init = false;
std::shared_ptr<Renderer> Renderer::renderer = nullptr;
GLFWwindow* Renderer::window = nullptr;

std::shared_ptr<Renderer> Renderer::GetRenderer()
{
    if (renderer == nullptr)
    {
        renderer = std::make_shared<Renderer>();
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

    // stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
}

void Renderer::DestroyRenderer()
{
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

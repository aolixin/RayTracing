//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include "stb_image.h"
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "shader.h"
//#include "camera.h"
//#include "model.h"
//#include <iostream>
//#include "Utils.h"
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//void renderSphere();
//
//// settings
//const unsigned int SCR_WIDTH = 1280;
//const unsigned int SCR_HEIGHT = 720;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = 800.0f / 2.0;
//float lastY = 600.0 / 2.0;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main()
//{
//    // glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_SAMPLES, 4);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//    glfwMakeContextCurrent(window);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
//    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    // build and compile shaders
//    // -------------------------
//    GLint envCubemap = buildEnvcubMap();
//    GLint irradianceMap = buildIrradianceMap(envCubemap);
//
//    Shader pbrShader("shaders/pbr.vert", "shaders/pbr.frag");
//    Shader backgroundShader("shaders/passToScreen.vert", "shaders/passToScreen.frag");
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//    pbrShader.use();
//    pbrShader.setInt("irradianceMap", 0);
//    pbrShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
//    pbrShader.setFloat("ao", 1.0f);
//
//
//
//    // lights
//    // ------
//    glm::vec3 lightPositions[] = {
//        glm::vec3(-10.0f,  10.0f, 10.0f),
//        glm::vec3(10.0f,  10.0f, 10.0f),
//        glm::vec3(-10.0f, -10.0f, 10.0f),
//        glm::vec3(10.0f, -10.0f, 10.0f),
//    };
//    glm::vec3 lightColors[] = {
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f)
//    };
//    int nrRows = 7;
//    int nrColumns = 7;
//    float spacing = 2.5;
//
//   
//
//    // initialize static shader uniforms before rendering
//    // --------------------------------------------------
//
//    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
//    int scrWidth, scrHeight;
//    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
//    glViewport(0, 0, scrWidth, scrHeight);
//
//    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//    pbrShader.use();
//    pbrShader.setMat4("projection", projection);
//
//    glEnable(GL_DEPTH_TEST);
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        // per-frame time logic
//        // --------------------
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        // render
//        // ------
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//        //glClearColor(0.8f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // render scene, supplying the convoluted irradiance map to the final shader.
//        // ------------------------------------------------------------------------------------------
//        pbrShader.use();
//        glm::mat4 view = camera.GetViewMatrix();
//        pbrShader.setMat4("view", view);
//        pbrShader.setVec3("camPos", camera.Position);
//
//        //// bind pre-computed IBL data
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//
//        //// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
//        glm::mat4 model = glm::mat4(1.0f);
//        for (int row = 0; row < nrRows; ++row)
//        {
//            pbrShader.setFloat("metallic", (float)row / (float)nrRows);
//            for (int col = 0; col < nrColumns; ++col)
//            {
//                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
//                // on direct lighting.
//                pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
//
//                model = glm::mat4(1.0f);
//                model = glm::translate(model, glm::vec3(
//                    (float)(col - (nrColumns / 2)) * spacing,
//                    (float)(row - (nrRows / 2)) * spacing,
//                    -2.0f
//                ));
//                pbrShader.setMat4("model", model);
//                pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
//                renderSphere();
//            }
//        }
//
//
//        //// render light source (simply re-render sphere at light positions)
//        //// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
//        //// keeps the codeprint small.
//        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
//        {
//            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
//            newPos = lightPositions[i];
//            pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
//            pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
//
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, newPos);
//            model = glm::scale(model, glm::vec3(0.5f));
//            pbrShader.setMat4("model", model);
//            pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
//            renderSphere();
//        }
//
//        // render skybox (render as last to prevent overdraw)
//
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        view = camera.GetViewMatrix();
//        backgroundShader.use();
//        backgroundShader.setMat4("view", view);
//        backgroundShader.setMat4("projection", projection);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//        backgroundShader.setInt("environmentMap", 0);
//        glEnable(GL_DEPTH_TEST);
//        drawCube(backgroundShader);
//
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glfwTerminate();
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
//// renders (and builds at first invocation) a sphere
//// -------------------------------------------------
//unsigned int sphereVAO = 0;
//unsigned int indexCount;
//void renderSphere()
//{
//    if (sphereVAO == 0)
//    {
//        glGenVertexArrays(1, &sphereVAO);
//
//        unsigned int vbo, ebo;
//        glGenBuffers(1, &vbo);
//        glGenBuffers(1, &ebo);
//
//        std::vector<glm::vec3> positions;
//        std::vector<glm::vec2> uv;
//        std::vector<glm::vec3> normals;
//        std::vector<unsigned int> indices;
//
//        const unsigned int X_SEGMENTS = 64;
//        const unsigned int Y_SEGMENTS = 64;
//        const float PI = 3.14159265359f;
//        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
//        {
//            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
//            {
//                float xSegment = (float)x / (float)X_SEGMENTS;
//                float ySegment = (float)y / (float)Y_SEGMENTS;
//                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
//                float yPos = std::cos(ySegment * PI);
//                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
//
//                positions.push_back(glm::vec3(xPos, yPos, zPos));
//                uv.push_back(glm::vec2(xSegment, ySegment));
//                normals.push_back(glm::vec3(xPos, yPos, zPos));
//            }
//        }
//
//        bool oddRow = false;
//        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
//        {
//            if (!oddRow) // even rows: y == 0, y == 2; and so on
//            {
//                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
//                {
//                    indices.push_back(y * (X_SEGMENTS + 1) + x);
//                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//                }
//            }
//            else
//            {
//                for (int x = X_SEGMENTS; x >= 0; --x)
//                {
//                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//                    indices.push_back(y * (X_SEGMENTS + 1) + x);
//                }
//            }
//            oddRow = !oddRow;
//        }
//        indexCount = static_cast<unsigned int>(indices.size());
//
//        std::vector<float> data;
//        for (unsigned int i = 0; i < positions.size(); ++i)
//        {
//            data.push_back(positions[i].x);
//            data.push_back(positions[i].y);
//            data.push_back(positions[i].z);
//            if (normals.size() > 0)
//            {
//                data.push_back(normals[i].x);
//                data.push_back(normals[i].y);
//                data.push_back(normals[i].z);
//            }
//            if (uv.size() > 0)
//            {
//                data.push_back(uv[i].x);
//                data.push_back(uv[i].y);
//            }
//        }
//        glBindVertexArray(sphereVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//        unsigned int stride = (3 + 2 + 3) * sizeof(float);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
//    }
//
//    glBindVertexArray(sphereVAO);
//    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
//}
//

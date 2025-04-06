#pragma once

shared_ptr<Scene> BuildScene()
{
    glm::mat4 identity = glm::mat4(1.0f);

    // bunny
    Shader shader0("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat0(shader0);
    mat0.baseColor = glm::vec3(0.2f, 0.2f, 0.8f);
    Model model0("Resources/models/bunny2.obj");

    // left
    Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat1(shader1);
    mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
    mat1.baseColor = glm::vec3(1);
    Model model1("Resources/models/cornellbox/left.obj");

    // right
    Shader shader2("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat2(shader2);
    mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    mat2.baseColor = glm::vec3(1);
    Model model2("Resources/models/cornellbox/right.obj");

    // floor
    Shader shader3("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat3(shader3);
    mat3.baseColor = glm::vec3(1.f, 1.f, 1.f);
    Model model3("Resources/models/cornellbox/floor.obj");
    
    // light
    Shader shader4("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat4(shader3);
    mat3.emissive = glm::vec3(0.8f, 0.8f, 0.8f);
    Model model4("Resources/models/cornellbox/light.obj");
    

    shared_ptr<Scene> myScene = make_shared<Scene>();

    myScene->Add(model0, mat0);
    myScene->Add(model1, mat1);
    myScene->Add(model2, mat2);
    myScene->Add(model3, mat3);
    myScene->Add(model4, mat4);

    return myScene;
}

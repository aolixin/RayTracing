#pragma once

shared_ptr<Scene> BuildScene()
{
    glm::mat4 identity = glm::mat4(1.0f);

    Shader shader0("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat0(shader0);
    Model model0("Resources/models/bunny.obj");
    glm::mat4 m0 = glm::scale(identity, glm::vec3(0.6f, 0.6f, 0.6f));

    Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat1(shader1);
    mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
    Model model1("Resources/models/cornellbox/left.obj");
    glm::mat4 m1 = glm::translate(identity, glm::vec3(-1.0f, 0.0f, 0.0f));

    Shader shader2("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat2(shader2);
    mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    Model model2("Resources/models/cornellbox/right.obj");
    glm::mat4 m2 = glm::translate(identity, glm::vec3(1.0f, 0.0f, 0.0f));

    Shader shader3("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat3(shader3);
    mat3.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    Model model3("Resources/models/cornellbox/floor.obj");

    glm::mat4 m3 = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));


    shared_ptr<Scene> myScene = make_shared<Scene>();
    myScene->Add(model0, mat0, m0);
    myScene->Add(model1, mat1, m1);
    myScene->Add(model2, mat2, m2);
    myScene->Add(model3, mat3, m3);

    return myScene;
}

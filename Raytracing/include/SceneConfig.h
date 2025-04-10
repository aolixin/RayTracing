#pragma once
#include "Utils.h"

shared_ptr<Scene> BuildScene()
{
    shared_ptr<Scene> myScene = make_shared<Scene>();
    glm::mat4 identity = glm::mat4(1.0f);

    // bunny
    Shader shader0("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat0(shader0);
    mat0.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    Model model0("Resources/models/bunny.obj");

    // left
    Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat1(shader1);
    mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
    Model model1("Resources/models/cornellbox/left.obj");

    // right
    Shader shader2("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat2(shader2);
    mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    Model model2("Resources/models/cornellbox/right.obj");

    // floor
    Shader shader3("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat3(shader3);
    mat3.baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
    mat3.roughness = 0.5f;
    mat3.metallic = 0.0f;
    Model model3("Resources/models/cornellbox/floor.obj");

    // light
    Shader shader4("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat4(shader4);
    mat4.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    mat4.emissive = glm::vec3(1.0f, 1.0f, 1.0f);
    Model model4("Resources/models/cornellbox/light.obj");

    // short
    Shader shader5("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat5(shader5);
    mat5.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    Model model5("Resources/models/cornellbox/short.obj");


    // tall
    Shader shader6("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat6(shader6);
    mat6.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    Model model6("Resources/models/cornellbox/tall.obj");

    // myScene->Add(model0, mat0);
    myScene->Add(model1, mat1);
    myScene->Add(model2, mat2);
    myScene->Add(model3, mat3);
    myScene->Add(model4, mat4);
    myScene->Add(model5, mat5);
    myScene->Add(model6, mat6);

    /*
    // spheres
    Shader shader7("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Model sphere("Resources/models/sphere.obj");

    // First sphere
    Material m(shader7);
    m.baseColor = vec3(1, 0.5, 0.5);
    m.metallic = 1;
    m.roughness = 0.1;
    glm::mat4 trans7 = glm::translate(identity, glm::vec3(-2.f, 0.0f, 0.0f));
    myScene->Add(sphere, m, trans7);

    // Second sphere
    m.baseColor = vec3(0.5, 0.5, 1);
    m.metallic = 1;
    m.roughness = 0.2;
    glm::mat4 trans8 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
    myScene->Add(sphere, m, trans8);

    // Third sphere
    m.baseColor = vec3(0.5, 1, 0.5);
    m.metallic = 1;
    m.roughness = 0.3;
    glm::mat4 trans9 = glm::translate(identity, glm::vec3(2.f, 0.0f, 0.0f));
    myScene->Add(sphere, m, trans9);
    //
    // // Fourth sphere
    // m.baseColor = vec3(1, 1, 0.5);
    // m.metallic = 0.0;
    // m.roughness = 0.1;
    // glm::mat4 trans10 = glm::translate(identity, glm::vec3(-2.f, 0.6f, 0.0f));
    // myScene->Add(sphere, m, trans10);
    //
    // // Fifth sphere
    // m.baseColor = vec3(0.5, 1, 1);
    // m.metallic = 0.0;
    // m.roughness = 0.1;
    // glm::mat4 trans11 = glm::translate(identity, glm::vec3(0.0f, 0.6f, 0.0f));
    // myScene->Add(sphere, m, trans11);
    //
    // // Sixth sphere
    // m.baseColor = vec3(1, 0.5, 1);
    // m.metallic = 0.0;
    // m.roughness = 0.1;
    // glm::mat4 trans12 = glm::translate(identity, glm::vec3(2.f, 0.6f, 0.0f));
    // myScene->Add(sphere, m, trans12);
*/

    int width, height;
    float* data = load_hdr_img("Resources/textures/hdr/test5.hdr", width, height);
    myScene->hdrMap = GenGpuTex(data, width, height);

    myScene->envCubeMap = buildEnvCubMap(data, width, height);

    data = calculateHdrCache(data, width, height);

    if (data)
        free(data);

    return myScene;
}

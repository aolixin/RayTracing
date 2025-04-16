#pragma once
#include "Utils.h"
#include "GlobalFeat.h"

shared_ptr<Scene> BuildCornellbox();
shared_ptr<Scene> BuildSphere();

// Debug
shared_ptr<Scene> BuildDebugBVHScene();

shared_ptr<Scene> BuildScene()
{
    if (RENDER_PATH == RenderPath::Forward || RENDER_PATH == RenderPath::GI)
    {
        return BuildCornellbox();
        // return BuildSphere();
    }
    else if (RENDER_PATH == RenderPath::DebugBVH)
    {
        // return BuildCornellbox();
        return BuildDebugBVHScene();
    }
    else
        return nullptr;
}


shared_ptr<Scene> BuildCornellbox()
{
    shared_ptr<Scene> myScene = make_shared<Scene>();
    glm::mat4 identity = glm::mat4(1.0f);

    // bunny
    Shader phongShader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    
    Material mat0(phongShader);
    mat0.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    mat0.specular = 0.0f;
    Model bunny("Resources/models/bunny.obj");

    // left
    Material mat1(phongShader);
    mat1.baseColor = glm::vec3(0.8f, 0.2f, 0.2f);
    mat1.specular = 0.0f;
    Model left("Resources/models/cornellbox/left.obj");

    // right
    Material mat2(phongShader);
    mat2.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    mat2.specular = 0.0f;
    Model right("Resources/models/cornellbox/right.obj");

    // floor
    Material mat3(phongShader);
    mat3.baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
    mat3.roughness = 0.5f;
    mat3.metallic = 0.0f;
    Model floor("Resources/models/cornellbox/floor.obj");

    // light
    Material mat4(phongShader);
    mat4.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    mat4.emissive = glm::vec3(1.0f, 1.0f, 1.0f);
    Model light("Resources/models/cornellbox/light.obj");

    // short
    Material mat5(phongShader);
    mat5.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    mat5.specular = 0.0f;
    Model shortBox("Resources/models/cornellbox/short.obj");


    // tall
    Material mat6(phongShader);
    mat6.baseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    mat6.specular = 0.0f;
    Model tallBox("Resources/models/cornellbox/tall.obj");

    myScene->Add(bunny, mat0);
    myScene->Add(left, mat1);
    myScene->Add(right, mat2);
    myScene->Add(floor, mat3);
    // myScene->Add(light, mat4);
    // myScene->Add(shortBox, mat5);
    // myScene->Add(tallBox, mat6);


    float* data = load_hdr_img("Resources/textures/hdr/test3.hdr", myScene->hdrWidth, myScene->hdrHeight);
    myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

    myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

    data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

    myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

    if (data)
        free(data);

    return myScene;
}


shared_ptr<Scene> BuildSphere()
{
    shared_ptr<Scene> myScene = make_shared<Scene>();
    glm::mat4 identity = glm::mat4(1.0f);

    // spheres
    Shader shader("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Model sphere("Resources/models/sphere.obj");

    // First sphere
    Material m(shader);
    // m.baseColor = vec3(1, 0.5, 0.5);
    // m.metallic = 0.9;
    // m.roughness = 0.1;
    // glm::mat4 trans7 = glm::translate(identity, glm::vec3(-1.3f, -0.6f, 0.0f));
    // myScene->Add(sphere, m, trans7);

    // Second sphere
    m.baseColor = vec3(1, 1, 1);
    m.metallic = 0.9;
    m.roughness = 0.2;
    glm::mat4 trans8 = glm::translate(identity, glm::vec3(0.0f, -0.6f, 0.0f));
    myScene->Add(sphere, m, trans8);

    // // Third sphere
    // m.baseColor = vec3(0.5, 1, 0.5);
    // m.metallic = 0.9;
    // m.roughness = 0.3;
    // glm::mat4 trans9 = glm::translate(identity, glm::vec3(1.3f, -0.6f, 0.0f));
    // myScene->Add(sphere, m, trans9);
    //
    // // Fourth sphere
    // m.baseColor = vec3(1, 1, 0.5);
    // m.metallic = 0.0;
    // m.roughness = 0.1;
    // glm::mat4 trans10 = glm::translate(identity, glm::vec3(-1.3f, 0.6f, 0.0f));
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
    // glm::mat4 trans12 = glm::translate(identity, glm::vec3(1.3f, 0.6f, 0.0f));
    // myScene->Add(sphere, m, trans12);


    // plane
    Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/phong.frag");
    Material mat1(shader1);
    mat1.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    mat1.specular = 0.9f;
    mat1.roughness = 0.4f;
    Model model1("Resources/models/plane.obj");
    myScene->Add(model1, mat1);

    float* data = load_hdr_img("Resources/textures/hdr/test4.hdr", myScene->hdrWidth, myScene->hdrHeight);
    myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

    myScene->hdrMap = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

    // data = calculateHdrCache(data, myScene->hdrWidth, myScene->hdrHeight);

    // myScene->hdrCache = GenGpuTex(data, myScene->hdrWidth, myScene->hdrHeight);

    if (data)
        free(data);

    return myScene;
}

shared_ptr<Scene> BuildDebugBVHScene()
{
    shared_ptr<Scene> myScene = make_shared<Scene>();
    glm::mat4 identity = glm::mat4(1.0f);

    // bunny
    Shader shader0("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");
    Material mat0(shader0);
    mat0.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    mat0.specular = 0.0f;
    Model model0("Resources/models/bunny.obj");
    myScene->Add(model0, mat0);

    // plane
    Shader shader1("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");
    Material mat1(shader1);
    mat1.baseColor = glm::vec3(0.2f, 0.8f, 0.2f);
    Model model1("Resources/models/plane.obj");
    myScene->Add(model1, mat1);

    float* data = load_hdr_img("Resources/textures/hdr/test4.hdr", myScene->hdrWidth, myScene->hdrHeight);
    myScene->envCubeMap = BuildEnvCubMap(data, myScene->hdrWidth, myScene->hdrHeight);

    if (data)
        free(data);

    return myScene;
}

#include "Scene.h"

void Scene::Add(Model model, Material material)
{
    // models.push_back(model);
    for (auto& mesh : model.meshes)
    {
        render_nodes.emplace_back(
            make_shared<Mesh>(mesh),
            make_shared<Material>(material)
        );
    }
}

void Scene::SetupGIScene()
{
    for (const auto& render_node : render_nodes)
    {
        int start = triangles_expand.size();
        triangles_expand.insert(triangles_expand.end(), render_node.mesh->triangles.begin(),
                                render_node.mesh->triangles.end());

        for(int i = start; i < triangles_expand.size(); i++)
        {
            triangles_expand[i].materialID = materials_count;
            materials_expand.push_back(*render_node.material);
            materials_count++;
        }
        
    }

    this->myBVH.triangles = this->triangles_expand;
    this->myBVH.buildBVH(0, this->triangles_expand.size() - 1);
    std::cout << "SAH BVHNode:    " << this->myBVH.nodes.size() << endl;
    GenBuffers();
}

// void Scene::Draw()
// {
//     for (int i = 0; i < models.size(); i++)
//     {
//         models[i].Draw();
//     }
// }

void Scene::GenBuffers()
{
    vector<Triangle_encoded> triangles_encoded(triangles_expand.size());
    for (int i = 0; i < triangles_expand.size(); i++)
    {
        Triangle& t = triangles_expand[i];

        triangles_encoded[i].p1 = t.vertex[0].Position;
        triangles_encoded[i].p2 = t.vertex[1].Position;
        triangles_encoded[i].p3 = t.vertex[2].Position;

        triangles_encoded[i].n1 = t.vertex[0].Normal;
        triangles_encoded[i].n2 = t.vertex[1].Normal;
        triangles_encoded[i].n3 = t.vertex[2].Normal;

        triangles_encoded[i].materialID = vec3(t.materialID, 0, 0);
    }

    vector<BVHNode_encoded> nodes_encoded(this->myBVH.nodes.size());
    for (int i = 0; i < this->myBVH.nodes.size(); i++)
    {
        nodes_encoded[i].childs = vec3(this->myBVH.nodes[i].left, this->myBVH.nodes[i].right, 0);
        nodes_encoded[i].leafInfo = vec3(this->myBVH.nodes[i].n, this->myBVH.nodes[i].index, 0);
        nodes_encoded[i].AA = this->myBVH.nodes[i].AA;
        nodes_encoded[i].BB = this->myBVH.nodes[i].BB;
    }


    vector<Material_encoded> materials_encoded(this->materials_expand.size());
    for (int i = 0; i < materials_encoded.size(); i++)
    {
        materials_encoded[i] = materials_expand[i].encoded();
    }

    GLuint tbo0;
    glGenBuffers(1, &tbo0);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
    glBufferData(GL_TEXTURE_BUFFER, triangles_encoded.size() * sizeof(Triangle_encoded), &triangles_encoded[0],
                 GL_STATIC_DRAW);
    glGenTextures(1, &trianglesTextureBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

    nTriangles = triangles_encoded.size();
    triangles_encoded.clear();

    GLuint tbo1;
    glGenBuffers(1, &tbo1);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
    glBufferData(GL_TEXTURE_BUFFER, nodes_encoded.size() * sizeof(BVHNode_encoded), &nodes_encoded[0],
                 GL_STATIC_DRAW);
    glGenTextures(1, &nodesTextureBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);

    nNodes = nodes_encoded.size();
    nodes_encoded.clear();

    GLuint tbo2;
    glGenBuffers(1, &tbo2);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo2);
    glBufferData(GL_TEXTURE_BUFFER, materials_encoded.size() * sizeof(Material_encoded), &materials_encoded[0],
                 GL_STATIC_DRAW);
    glGenTextures(1, &materialsTextureBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, materialsTextureBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo2);

    nMaterials = materials_encoded.size();
    materials_encoded.clear();

    // for (int i = 0; i < models.size(); i++)
    // {
    //     models[i].setupDataOfShader(trianglesTextureBuffer, nodesTextureBuffer, materialsTextureBuffer);
    // }
}

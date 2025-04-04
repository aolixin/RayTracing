#include "Mesh.h"


// constructor
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->myShader = (Shader*)malloc(sizeof(Shader));
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    SetupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<Triangle> triangles)
{
    this->myShader = (Shader*)malloc(sizeof(Shader));
    std::cout << "vertices num: " << vertices.size() << "    triangles num: " << triangles.size() << endl;
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->triangles = triangles;


    //this->myBVH.buildBVHwithSAH(0,this->triangles.size()-1);

    //std::cout << "buildBVHwithSAH????  " << endl;
    //std::cout <<"SAH BVHNode????:    " << this->myBVH.nodes.size() << endl;

    SetupMesh();
}

void Mesh::BuildBVH()
{
    this->myBVH.triangles = this->triangles;
    this->myBVH.buildBVH(0, this->triangles.size() - 1);
}

// void Mesh::setupMaterial(Material material)
// {
//     this->material = material;
// }

// void Mesh::setShader(Shader& shader)
// {
//     this->myShader = &shader;
// }

// void Mesh::setupDataOfShader(GLint trianglesTextureBuffer, GLint nodesTextureBuffer, GLint materialsTextureBuffer)
// {
//     // Shader shader = *this->myShader;
//     // glActiveTexture(GL_TEXTURE2);
//     // glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
//     // glUniform1i(glGetUniformLocation(shader.ID, "triangles"), 2);
//     //
//     // glActiveTexture(GL_TEXTURE3);
//     // glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
//     // glUniform1i(glGetUniformLocation(shader.ID, "nodes"), 3);
//     //
//     // glActiveTexture(GL_TEXTURE4);
//     // glBindTexture(GL_TEXTURE_BUFFER, materialsTextureBuffer);
//     // glUniform1i(glGetUniformLocation(shader.ID, "materials"), 4);
//     this->trianglesTextureBuffer = trianglesTextureBuffer;
//     this->nodesTextureBuffer = nodesTextureBuffer;
//     this->materialsTextureBuffer = materialsTextureBuffer;
// }

// render the mesh
void Mesh::Draw(Shader& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    //Material material;
    //material.baseColor = vec3(1, 0, 0);

    shader.setVec3("material.baseColor", material.baseColor);
    shader.setVec3("material.emissive", material.emissive);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
    glUniform1i(glGetUniformLocation(shader.ID, "triangles"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
    glUniform1i(glGetUniformLocation(shader.ID, "nodes"), 3);


    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw()
{
    Shader shader = *this->myShader;
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
    glUniform1i(glGetUniformLocation(shader.ID, "triangles"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
    glUniform1i(glGetUniformLocation(shader.ID, "nodes"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_BUFFER, materialsTextureBuffer);
    glUniform1i(glGetUniformLocation(shader.ID, "materials"), 4);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}


// initializes all the buffer objects/arrays
void Mesh::SetupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights5
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

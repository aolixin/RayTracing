#pragma once
#include "Model.h"


using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);


// model data 
vector<Texture> textures_loaded;
// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
vector<Mesh> meshes;
string directory;
bool gammaCorrection;

//Shader *myShader;

// constructor, expects a filepath to a 3D model.
Model::Model(string const& path, bool gamma) : gammaCorrection(gamma)
{
    loadModel(path);
}

// void Model::BuildBVH()
// {
//     for (unsigned int i = 0; i < meshes.size(); i++)
//         meshes[i].BuildBVH();
// }

// draws the model, and thus all its meshes
// void Model::Draw(Shader& shader)
// {
//     for (unsigned int i = 0; i < meshes.size(); i++)
//         meshes[i].Draw(shader);
//     //meshes[i].Draw(*this->myShader);
// }

// void Model::Draw()
// {
//     for (unsigned int i = 0; i < meshes.size(); i++)
//         meshes[i].Draw();
// }

// void Model::setupDataOfShader(GLint trianglesTextureBuffer, GLint nodesTextureBuffer, GLint materialsTextureBuffer)
// {
//     for (int i = 0; i < meshes.size(); i++)
//     {
//         meshes[i].setupDataOfShader(trianglesTextureBuffer, nodesTextureBuffer, materialsTextureBuffer);
//     }
// }

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // Find the bounding box of the model
    glm::vec3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    findBoundingBox(scene->mRootNode, scene, minBounds, maxBounds);

    // Calculate the center and scale for normalization
    glm::vec3 center = (minBounds + maxBounds) / 2.0f;
    float scaleFactor = 2.0f / glm::max(glm::max(maxBounds.x - minBounds.x, maxBounds.y - minBounds.y),
                                        maxBounds.z - minBounds.z);

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, center, scaleFactor);
}

void Model::findBoundingBox(aiNode* node, const aiScene* scene, glm::vec3& minBounds, glm::vec3& maxBounds)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            glm::vec3 vertexPos(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            minBounds = glm::min(minBounds, vertexPos);
            maxBounds = glm::max(maxBounds, vertexPos);
        }
    }
    // recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        findBoundingBox(node->mChildren[i], scene, minBounds, maxBounds);
    }
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene, const glm::vec3& center, float scaleFactor)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, center, scaleFactor));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, center, scaleFactor);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::vec3& center, float scaleFactor)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    vector<Triangle> triangles;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        // vector.x = (mesh->mVertices[i].x - center.x) * scaleFactor;
        // vector.y = (mesh->mVertices[i].y - center.y) * scaleFactor;
        // vector.z = (mesh->mVertices[i].z - center.z) * scaleFactor;

        vector.x = (mesh->mVertices[i].x);
        vector.y = (mesh->mVertices[i].y);
        vector.z = (mesh->mVertices[i].z);
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    std::cout << "faces num: " << mesh->mNumFaces << endl;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];


        Triangle atriangle;
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
            atriangle.vertex.push_back(vertices[face.mIndices[j]]);
        }
        triangles.push_back(atriangle);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    //return Mesh(vertices, indices, textures);
    return Mesh(vertices, indices, textures, triangles);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {
            // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
            // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

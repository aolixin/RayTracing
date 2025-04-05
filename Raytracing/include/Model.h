#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>
#include "stb_image.h"

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);


class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    //Shader *myShader;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false);

    // void BuildBVH();


    // draws the model, and thus all its meshes
    // void Draw(Shader& shader);
    // void Draw();

    // void setupDataOfShader(GLint trianglesTextureBuffer, GLint nodesTextureBuffer, GLint materialsTextureBuffer);

private:
    void loadModel(string const& path);


    void findBoundingBox(aiNode* node, const aiScene* scene, glm::vec3& minBounds, glm::vec3& maxBounds);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, const glm::vec3& center, float scaleFactor);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::vec3& center, float scaleFactor);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

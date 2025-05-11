#pragma once


#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <vector>
#include "Material.h"
#include "Triangle.h"
#include "BVH.h"
using namespace std;

#define MAX_BONE_INFLUENCE 4



struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	bool hasPassedTriangles = false;
	// mesh Data
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;

	Material             material;

	vector<Triangle>     triangles;

	// BVH                  myBVH;

	GLuint VAO;

	//Shader* myShader;

	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<Triangle>triangles);

	~Mesh();
private:
	// render data 


	// initializes all the buffer objects/arrays
	void SetupMesh();
	GLuint VBO, EBO;

};
#pragma once


#include <glad/glad.h>
#include "Shader.h"
#include "stb_image.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

extern GLfloat cubeVertices[];

extern GLuint cubeIndices[];



void drawCube(Shader shader);

unsigned int load_hdr_img(std::string path);

GLint buildEnvCubMap();

GLint buildIrradianceMap(GLint envCubeMap = -9999);


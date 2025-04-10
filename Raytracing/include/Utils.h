#pragma once


#include <vector>
#include <glad/glad.h>
#include "Shader.h"
#include "stb_image.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

extern GLfloat cubeVertices[];
extern GLuint cubeIndices[];

extern GLfloat quadVertices[];
extern GLuint quadIndices[];


void DrawCube(const Shader& shader);

void DrawQuad(const Shader& shader);

GLuint GetFrameBuffer(int SCR_WIDTH, int SCR_HEIGHT, std::vector<GLuint>& frameTextures,
                      int nColorAttachments = 1, int nDepthAttachments = 0);

float* load_hdr_img(std::string path, int& width, int& height);

GLuint buildEnvCubMap(float* data, int width, int height);

float* calculateHdrCache(float* HDR, int width, int height);

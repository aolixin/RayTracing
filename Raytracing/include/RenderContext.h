#pragma once

#include <glm/glm.hpp>

struct RenderContext
{
    glm::vec3 viewPos;
    glm::mat4 projection;
    glm::mat4 view;
};

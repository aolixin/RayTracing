#pragma once
#include "RenderContext.h"

class Mesh;
class Material;

struct RenderNode
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    glm::mat4 modelMatrix;

    RenderNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
               glm::mat4 modelMatrix = glm::mat4(1.0f))
    {
        this->mesh = mesh;
        this->material = material;
        this->modelMatrix = modelMatrix;
    }

    inline void Draw(const RenderContext& context)
    {
        // bind appropriate textures

        material->shader->use();
        glm::mat4 projection = context.projection;
        glm::mat4 view = context.view;
        material->shader->setMat4("projection", projection);
        material->shader->setMat4("view", view);
        material->shader->setMat4("model", modelMatrix);
        material->shader->setVec3("viewPos", context.viewPos);

        material->shader->setVec3("objectColor", material->baseColor);

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < mesh->textures.size(); i++)
        {
            // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = mesh->textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            material->shader->setTexture(name + number, mesh->textures[i].id, i);
        }

        // draw mesh
        glBindVertexArray(mesh->VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);\
        glBindVertexArray(0);
       
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
};

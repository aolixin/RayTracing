#pragma once
#include <memory>
#include "Mesh.h"

class Material;

struct RenderNode
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    RenderNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : mesh(mesh), material(material)
    {
    }

    inline void Draw()
    {
        // mesh->Draw();

        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < mesh->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
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

            // now set the sampler to the correct texture unit
            // glUniform1i(glGetUniformLocation(material->shader->ID, (name + number).c_str()), i);
            // // and finally bind the texture
            // glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
            material->shader->setTexture(name + number, mesh->textures[i].id, i);
        }

        //Material material;
        //material.baseColor = vec3(1, 0, 0);

        material->shader->setVec3("material.baseColor", material->baseColor);
        material->shader->setVec3("material.emissive", material->emissive);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_BUFFER, mesh->trianglesTextureBuffer);
        glUniform1i(glGetUniformLocation(material->shader->ID, "triangles"), 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_BUFFER, mesh->nodesTextureBuffer);
        glUniform1i(glGetUniformLocation(material->shader->ID, "nodes"), 3);


        // draw mesh
        glBindVertexArray(mesh->VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
};

#pragma once
#include <memory>

class Mesh;
class Material;

struct RenderNode
{
    std::shared_ptr<Mesh>mesh;
    std::shared_ptr<Material>material;

    RenderNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : mesh(mesh), material(material) {}
};

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

namespace Engine
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec2 tangent;
    };

    class Mesh
    {
    public:
        void Init(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader &shader);
        void Clear();

    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

    private:
        void Setup();

    private:
        unsigned int VBO_;
        unsigned int VAO_;
        unsigned int EBO_;
    };
}

#endif
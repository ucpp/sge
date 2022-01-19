#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

namespace sge
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
        void initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures);
        void draw(Shader &shader);
        void clear();

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;

    private:
        void setup();

    private:
        uint32_t VBO;
        uint32_t VAO;
        uint32_t EBO;
    };
}
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
		glm::vec3 color;
	};

	class Mesh
	{
	public:
		void initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures, glm::vec4 color);
		void draw(Shader& shader, int cubemap_texture = -1);
		void clear();

		uint32_t getCountVertices() const;

	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;
		glm::vec4 color;

	private:
		void setup();

	private:
		uint32_t VBO;
		uint32_t VAO;
		uint32_t EBO;
	};
}
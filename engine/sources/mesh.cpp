#include "mesh.h"

#include <glad/glad.h>

namespace sge
{
	void Mesh::initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setup();
	}

	void Mesh::setup()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);
	}

	void Mesh::draw(Shader& shader, int cubemap_texture)
	{
		for (uint32_t i = 0; i < textures.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			shader.setInt(textures[i].type.c_str(), i);
			textures[i].bind();
		}

		glBindVertexArray(VAO);
		if (cubemap_texture >= 0)
		{
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
		}

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

	void Mesh::clear()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	uint32_t Mesh::getCountVertices() const
	{
		return vertices.size();
	}
}
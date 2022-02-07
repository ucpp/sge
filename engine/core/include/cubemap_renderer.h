#pragma once

#include <cstdint>
#include "shader.h"

namespace sge
{
	class CubemapRenderer
	{
	public:
		void initialize(const std::string& cubemap_name, const std::string& shader_name);
		void render(const glm::mat4& view, const glm::mat4& projection);
		void shutdown();

	public:
		uint32_t cubemap_texture = -1;

	private:
		Shader shader;
		uint32_t VAO;
		uint32_t VBO;
	};
}
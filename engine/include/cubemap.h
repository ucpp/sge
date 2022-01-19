#pragma once

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace sge
{
	class Cubemap
	{
	public:
		Cubemap();

		void generate(const std::vector<unsigned char*>& data, const std::vector<glm::ivec2>& sizes);
		void bind() const;

	public:
		uint32_t id;
	};
}
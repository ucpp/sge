#include "cubemap.h"

#include <glad/glad.h>

namespace sge
{
	Cubemap::Cubemap()
	{
		glGenTextures(1, &id);
	}

	void Cubemap::generate(const std::vector<unsigned char*>& data, const std::vector<glm::ivec2>& sizes)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (uint32_t i = 0; i < data.size(); ++i)
		{
			int w = static_cast<int>(sizes[i].x);
			int h = static_cast<int>(sizes[i].y);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void Cubemap::bind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}
}
#include "texture.h"
#include <iostream>

#include <glad/glad.h>

namespace sge
{
	Texture::Texture()
	{
		width = 0;
		height = 0;
		internal_format = GL_RGB;
		image_format = GL_RGB;
		wrap_format = GL_REPEAT;
		glGenTextures(1, &id);
	}

	void Texture::generate(const unsigned char* data, uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, this->width, this->height, 0, image_format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_format);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_format);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture::setFormat(int channels, size_t pixel_size)
	{
		uint32_t format = GL_RGB;

		if (channels == 1)
		{
			format = GL_R;
		}
		else if (channels == 4)
		{
			format = GL_RGBA;
		}

		internal_format = format;
		image_format = format;
	}

	uint32_t Texture::getWidth() const
	{
		return width;
	}

	uint32_t Texture::getHeight() const
	{
		return height;
	}

	//TODO:destructor
}
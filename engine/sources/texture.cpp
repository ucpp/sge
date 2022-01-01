#include "texture.h"

#include <glad/glad.h>

namespace Engine
{
    Texture::Texture()
    {
        width_ = 0;
        height_ = 0;
        internal_format = GL_RGB;
        image_format = GL_RGB;

        glGenTextures(1, &Id);
    }

    void Texture::Generate(unsigned char* data, unsigned int width, unsigned int height)
    {
        width_ = width;
        height_ = height;

        glBindTexture(GL_TEXTURE_2D, Id);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width_, height_, 0, image_format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, Id);
    }

    unsigned int Texture::GetWidth() const
    {
        return width_;
    }

    unsigned int Texture::GetHeight() const
    {
        return height_;
    }
}
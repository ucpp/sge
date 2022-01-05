#include "texture.h"

#include <glad/glad.h>

namespace Engine
{
    Texture::Texture()
    {
        width_ = 0;
        height_ = 0;
        internal_format_ = GL_RGB;
        image_format_ = GL_RGB;

        glGenTextures(1, &id);
    }

    void Texture::Generate(unsigned char *data, unsigned int width, unsigned int height)
    {
        width_ = width;
        height_ = height;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width_, height_, 0, image_format_, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Texture::SetFormat(int channels, size_t pixel_size)
    {
        unsigned int format = GL_RGB;
        /*
        if(pixel_size == sizeof(float))
        {
            switch (channels)
            {
            case 1: format = GL_R32F; break;
            case 2: format = GL_RG32F; break;
            case 3: format = GL_RGB32F; break;
            case 4: format = GL_RGBA32F; break;
            }
        }
        else
        {
            switch (channels)
            {
            case 1: format = GL_R8; break;
            case 2: format = GL_RG8; break;
            case 3: format = GL_RGB8; break;
            case 4: format = GL_RGBA8; break;
            }
        }
        */
        if(channels == 1)
        {
            format = GL_R;
        }
        if(channels == 4)
        {
            format = GL_RGBA;
        }
        internal_format_ = format;
        image_format_ = format;
    }

    unsigned int Texture::GetWidth() const
    {
        return width_;
    }

    unsigned int Texture::GetHeight() const
    {
        return height_;
    }

    void Texture::EnableAlpha()
    {
        internal_format_ = GL_RGBA;
        image_format_ = GL_RGBA;
    }
}
#pragma once

#include <string>

namespace sge
{
    class Texture
    {
    public:
        Texture();
        void Generate(unsigned char *data, unsigned int width, unsigned int height);
        void Bind() const;

        void EnableAlpha();
        void SetFormat(int channels, size_t pixel_size);
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;

    public:
        unsigned int id;
        std::string type;

    private:
        unsigned int width_;
        unsigned int height_;
        unsigned int internal_format_;
        unsigned int image_format_;
        unsigned int wrap_format_;
    };
}
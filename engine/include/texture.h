#ifndef TEXTURE_H
#define TEXTURE_h

namespace Engine
{
    class Texture
    {
    public: 
        Texture();
        void Generate(unsigned char* data, unsigned int width, unsigned int height);
        void Bind() const;

        unsigned int Id;
        unsigned int internal_format;
        unsigned int image_format;
        
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;

    private:
        unsigned int width_;
        unsigned int height_;
    };
}

#endif
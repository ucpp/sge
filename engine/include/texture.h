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

        void EnableAlpha();
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
    public:
        unsigned int id;

    private:
        unsigned int width_;
        unsigned int height_;
        unsigned int internal_format_;
        unsigned int image_format_;
    };
}

#endif
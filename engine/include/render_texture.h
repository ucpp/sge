#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

namespace Engine
{
    class RenderTexture
    {
    public:
        void Init(int size);
        void Bind();
        void BindTexture(int texture_number);
        void Clear();

    private:
        unsigned int FBO_;
        unsigned int id_;
        int size_;
    };
}

#endif
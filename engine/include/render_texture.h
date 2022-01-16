#pragma once

namespace sge
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
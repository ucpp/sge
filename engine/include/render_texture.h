#pragma once
#include <stdint.h>

namespace sge
{
    class RenderTexture
    {
    public:
        void initialize(int size);
        void bind();
        void bindTexture(int texture_number);
        void clear();

    private:
        uint32_t FBO;
        uint32_t id;
        int size;
    };
}
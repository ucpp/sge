#ifndef _SGE_TEXTURE_MANAGER_H_
#define _SGE_TEXTURE_MANAGER_H_

#include "pch.h"
#include "sge_texture.h"

namespace SGE
{
    class TextureManager
    {
    public:
        static Texture* GetTexture(const std::string& texturePath, class Device* device, class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

    private:
        static std::unordered_map<std::string, std::unique_ptr<Texture>> m_textureCache;
    };
}

#endif // !_SGE_TEXTURE_MANAGER_H_
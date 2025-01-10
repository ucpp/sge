#ifndef _SGE_TEXTURE_MANAGER_H_
#define _SGE_TEXTURE_MANAGER_H_

#include "pch.h"
#include "sge_texture.h"
#include "sge_device.h"

namespace SGE
{
    class TextureManager
    {
    public:
        static uint32 GetTextureIndex(const std::string& texturePath, class Device* device, class DescriptorHeap* descriptorHeap);

    private:
        struct TextureData
        {
            std::unique_ptr<Texture> texture;
            uint32 descriptorIndex;
        };

        static std::unordered_map<std::string, TextureData> m_textureCache;

        static constexpr uint32 TextureHeapStartIndex = Device::CbvSrvHeapCapacity / 2;
        static uint32 m_currentTextureIndex;
    };
}

#endif // !_SGE_TEXTURE_MANAGER_H_

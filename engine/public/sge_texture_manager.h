#ifndef _SGE_TEXTURE_MANAGER_H_
#define _SGE_TEXTURE_MANAGER_H_

#include "pch.h"
#include "sge_texture.h"

namespace SGE
{
    class TextureManager
    {
    public:
        static uint32 GetTextureIndex(const std::string& texturePath, TextureType type, const class Device* device, const class DescriptorHeap* descriptorHeap);

    private:
        static void CreateDefaultTextures(const class Device* device, const class DescriptorHeap* descriptorHeap);

    private:
        struct TextureData
        {
            std::unique_ptr<Texture> texture;
            uint32 descriptorIndex;
        };

        static std::unordered_map<std::string, TextureData> m_textureCache;
        static std::unordered_map<TextureType, std::unique_ptr<Texture>> m_defaultTextures;
        static uint32 m_currentTextureIndex;
        static bool hasDefaultTextures;
    };
}

#endif // !_SGE_TEXTURE_MANAGER_H_

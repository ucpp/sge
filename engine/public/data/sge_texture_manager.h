#ifndef _SGE_TEXTURE_MANAGER_H_
#define _SGE_TEXTURE_MANAGER_H_

#include "pch.h"
#include "data/sge_texture.h"
#include "data/sge_cubemap_texture.h"
#include "data/sge_data_structures.h"

namespace SGE
{
    class TextureManager
    {
    public:
        static uint32 GetTextureIndex(const std::string& texturePath, TextureType type, const class Device* device, const class DescriptorHeap* descriptorHeap);
        static uint32 GetCubemapIndex(const CubemapAssetData& cubemapData, const class Device* device, const class DescriptorHeap* descriptorHeap);

    private:
        static void CreateDefaultTextures(const class Device* device, const class DescriptorHeap* descriptorHeap);

    private:
        struct TextureData
        {
            std::unique_ptr<Texture> texture;
            uint32 descriptorIndex;
        };

        struct CubemapData
        {
            std::unique_ptr<CubemapTexture> cubemap;
            uint32 descriptorIndex;
        };

        static std::unordered_map<std::string, TextureData> m_textureCache;
        static std::unordered_map<std::string, CubemapData> m_cubemapCache;
        static std::unordered_map<TextureType, std::unique_ptr<Texture>> m_defaultTextures;
        static uint32 m_currentTextureIndex;
        static bool hasDefaultTextures;
    };
}

#endif // !_SGE_TEXTURE_MANAGER_H_
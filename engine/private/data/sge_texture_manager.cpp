#include "data/sge_texture_manager.h"

#include <filesystem>
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"

namespace SGE
{
    std::unordered_map<std::string, TextureManager::TextureData> TextureManager::m_textureCache;
    std::unordered_map<std::string, TextureManager::CubemapData> TextureManager::m_cubemapCache;
    std::unordered_map<TextureType, std::unique_ptr<Texture>> TextureManager::m_defaultTextures;
    uint32 TextureManager::m_currentTextureIndex = TEXTURES_START_HEAP_INDEX;
    bool TextureManager::hasDefaultTextures = false;

    uint32 TextureManager::GetTextureIndex(const std::string& texturePath, TextureType type, const Device* device, const DescriptorHeap* descriptorHeap)
    {
        auto it = m_textureCache.find(texturePath);
        if (it != m_textureCache.end())
        {
            return it->second.descriptorIndex;
        }

        if (texturePath == "" || !std::filesystem::exists(texturePath))
        {
            if (!hasDefaultTextures)
            {
                CreateDefaultTextures(device, descriptorHeap);
            }

            return m_defaultTextures[type]->GetDescriptorIndex();
        }

        if (m_currentTextureIndex >= CBV_SRV_HEAP_CAPACITY)
        {
            throw std::runtime_error("TextureManager: Descriptor heap capacity exceeded!");
        }

        auto texture = std::make_unique<Texture>();
        uint32 descriptorIndex = m_currentTextureIndex;

        texture->Initialize(texturePath, device, descriptorHeap, descriptorIndex);

        m_textureCache[texturePath] = { std::move(texture), descriptorIndex };
        m_currentTextureIndex++;

        return descriptorIndex;
    }

    uint32 TextureManager::GetCubemapIndex(const CubemapAssetData& cubemapData, const Device* device, const DescriptorHeap* descriptorHeap)
    {
        std::string cubemapKey = cubemapData.right + cubemapData.left + cubemapData.top + cubemapData.bottom + cubemapData.front + cubemapData.back;

        auto it = m_cubemapCache.find(cubemapKey);
        if (it != m_cubemapCache.end())
        {
            return it->second.descriptorIndex;
        }

        if (m_currentTextureIndex >= CBV_SRV_HEAP_CAPACITY)
        {
            throw std::runtime_error("TextureManager: Descriptor heap capacity exceeded!");
        }

        auto cubemap = std::make_unique<CubemapTexture>();
        uint32 descriptorIndex = m_currentTextureIndex;

        cubemap->Initialize(cubemapData, device, descriptorHeap, descriptorIndex);

        m_cubemapCache[cubemapKey] = { std::move(cubemap), descriptorIndex };
        m_currentTextureIndex++;

        return descriptorIndex;
    }

    void TextureManager::CreateDefaultTextures(const Device* device, const DescriptorHeap* descriptorHeap)
    {
        m_defaultTextures[TextureType::Albedo] = std::make_unique<Texture>();
        m_defaultTextures[TextureType::Albedo]->CreateDefaultAlbedo(device, descriptorHeap, m_currentTextureIndex++);

        m_defaultTextures[TextureType::Metallic] = std::make_unique<Texture>();
        m_defaultTextures[TextureType::Metallic]->CreateDefaultMetallic(device, descriptorHeap, m_currentTextureIndex++);

        m_defaultTextures[TextureType::Roughness] = std::make_unique<Texture>();
        m_defaultTextures[TextureType::Roughness]->CreateDefaultRoughness(device, descriptorHeap, m_currentTextureIndex++);

        m_defaultTextures[TextureType::Normal] = std::make_unique<Texture>();
        m_defaultTextures[TextureType::Normal]->CreateDefaultNormal(device, descriptorHeap, m_currentTextureIndex++);

        m_defaultTextures[TextureType::Default] = std::make_unique<Texture>();
        m_defaultTextures[TextureType::Default]->CreateDefaultAlbedo(device, descriptorHeap, m_currentTextureIndex++);

        hasDefaultTextures = true;
    }
}
#include "sge_texture_manager.h"
#include "sge_device.h"
#include "sge_descriptor_heap.h"

namespace SGE
{
    std::unordered_map<std::string, TextureManager::TextureData> TextureManager::m_textureCache;
    uint32 TextureManager::m_currentTextureIndex = TextureHeapStartIndex;

    uint32 TextureManager::GetTextureIndex(const std::string& texturePath, Device* device, DescriptorHeap* descriptorHeap)
    {
        auto it = m_textureCache.find(texturePath);
        if (it != m_textureCache.end())
        {
            return it->second.descriptorIndex;
        }

        if (m_currentTextureIndex >= Device::CbvSrvHeapCapacity)
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
}

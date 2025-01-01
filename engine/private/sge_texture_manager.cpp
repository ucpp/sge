#include "sge_texture_manager.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::m_textureCache;

    Texture* TextureManager::GetTexture(const std::string& texturePath, Device* device, DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        auto it = m_textureCache.find(texturePath);
        if (it != m_textureCache.end())
        {
            return it->second.get();
        }

        auto texture = std::make_unique<Texture>();
        texture->Initialize(texturePath, device, descriptorHeap, descriptorIndex);
        Texture* texturePtr = texture.get();

        m_textureCache[texturePath] = std::move(texture);

        return texturePtr;
    }
}
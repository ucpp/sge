#ifndef _SGE_TEXTURE_H_
#define _SGE_TEXTURE_H_

#include "pch.h"

namespace SGE
{
    enum class TextureType
    {
        Albedo,
        Metallic,
        Roughness,
        Normal,
        Default
    };

    class Texture
    {
    public:
        void Initialize(const std::string& texturePath, const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        uint32 GetDescriptorIndex() const { return m_descriptorIndex; }

        void CreateDefaultAlbedo(const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        void CreateDefaultMetallic(const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        void CreateDefaultRoughness(const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        void CreateDefaultNormal(const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

    private:
        void CreateSinglePixelTexture(uint32 color, const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

    private:
        ComPtr<ID3D12Resource> m_resource = nullptr;
        ComPtr<ID3D12Resource> m_resourceUpload = nullptr;
        uint32 m_descriptorIndex = 0;
    };
}

#endif // !_SGE_TEXTURE_H_

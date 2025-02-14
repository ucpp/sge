#ifndef _SGE_CUBEMAP_TEXTURE_H_
#define _SGE_CUBEMAP_TEXTURE_H_

#include "pch.h"
#include "data/sge_data_structures.h"
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_helpers.h"
#include <DirectXTex.h>

namespace SGE
{
    class CubemapTexture
    {
    public:
        void Initialize(const CubemapAssetData& assetData, const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

        uint32 GetDescriptorIndex() const { return m_descriptorIndex; }

    private:
        void CreateCubeMapTexture(const std::array<std::string, 6>& texturePaths, const class Device* device, const class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);

    private:
        ComPtr<ID3D12Resource> m_resource = nullptr;
        ComPtr<ID3D12Resource> m_resourceUpload = nullptr;
        uint32 m_descriptorIndex = 0;
    };
}

#endif // !_SGE_CUBEMAP_TEXTURE_H_
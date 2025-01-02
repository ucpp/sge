#include "sge_texture.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_helpers.h"
#include <filesystem>
#include <DirectXTex.h>

namespace SGE
{
    void Texture::Initialize(const std::string& texturePath, Device* device, DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        if (!std::filesystem::exists(texturePath))
        {
            throw std::runtime_error("Texture file does not exist: " + texturePath);
        }

        TexMetadata metadata;
        ScratchImage scratchImage;

        std::filesystem::path filePath = texturePath;
        std::wstring fileName = filePath.wstring();

        if (filePath.extension() == L".dds")
        {
            Verify(LoadFromDDSFile(fileName.c_str(), DDS_FLAGS_NONE, &metadata, scratchImage)); 
        }
        else if (filePath.extension() == L".hdr")
        {
            Verify(LoadFromHDRFile(fileName.c_str(), &metadata, scratchImage));
        }
        else if (filePath.extension() == L".tga")
        {
            Verify(LoadFromTGAFile(fileName.c_str(), &metadata, scratchImage));
        }
        else
        {
            Verify(LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
        }

        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureDesc.Alignment = 0;
        textureDesc.Width = metadata.width;
        textureDesc.Height = static_cast<uint32>(metadata.height);
        textureDesc.DepthOrArraySize = 1;
        textureDesc.MipLevels = static_cast<uint16>(metadata.mipLevels); 
        textureDesc.Format = metadata.format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        Verify(device->GetDevice().Get()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                 D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_resource)));

        const uint32 subresourceCount = textureDesc.DepthOrArraySize * textureDesc.MipLevels;
        const uint64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, subresourceCount);
        Verify(device->GetDevice().Get()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
                                                                 &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_resourceUpload)));

        ID3D12GraphicsCommandList* commandList = device->GetCommandList().Get();

        std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(metadata.mipLevels);

        for (uint32 i = 0; i < metadata.mipLevels; ++i) 
        {
            auto mipmap = scratchImage.GetImage(i, 0, 0);
            subresourceData[i].pData = mipmap->pixels;
            subresourceData[i].RowPitch = mipmap->rowPitch;
            subresourceData[i].SlicePitch = mipmap->slicePitch;
        }

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &barrier);

        UpdateSubresources(commandList, m_resource.Get(), m_resourceUpload.Get(), 0, 0, subresourceCount, subresourceData.data());

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = static_cast<uint32>(metadata.mipLevels); 
        srvDesc.Texture2D.MostDetailedMip = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = descriptorHeap->GetCPUHandle(descriptorIndex);

        device->GetDevice()->CreateShaderResourceView(m_resource.Get(), &srvDesc, srvHandle);
    }
}
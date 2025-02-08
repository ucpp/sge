#include "data/sge_texture.h"

#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_helpers.h"
#include <filesystem>
#include <DirectXTex.h>

using namespace DirectX;

namespace SGE
{
    void Texture::Initialize(const std::string& texturePath, const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
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

        Verify(device->GetDevice().Get()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, IID_PPV_ARGS(&m_resource)));

        const uint32 subresourceCount = textureDesc.DepthOrArraySize * textureDesc.MipLevels;
        const uint64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, subresourceCount);
        
        Verify(device->GetDevice().Get()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), 
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, IID_PPV_ARGS(&m_resourceUpload)));

        ID3D12GraphicsCommandList* commandList = device->GetCommandList().Get();

        std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(metadata.mipLevels);

        for (uint32 i = 0; i < metadata.mipLevels; ++i) 
        {
            auto mipmap = scratchImage.GetImage(i, 0, 0);
            subresourceData[i].pData = mipmap->pixels;
            subresourceData[i].RowPitch = mipmap->rowPitch;
            subresourceData[i].SlicePitch = mipmap->slicePitch;
        }

        UpdateSubresources(commandList, m_resource.Get(), m_resourceUpload.Get(), 0, 0, subresourceCount, subresourceData.data());

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = metadata.format == DXGI_FORMAT_UNKNOWN ? DXGI_FORMAT_R8G8B8A8_UNORM : metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = static_cast<uint32>(metadata.mipLevels); 
        srvDesc.Texture2D.MostDetailedMip = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = descriptorHeap->GetCPUHandle(descriptorIndex);

        device->GetDevice()->CreateShaderResourceView(m_resource.Get(), &srvDesc, srvHandle);

        m_descriptorIndex = descriptorIndex;
    }

    void Texture::CreateSinglePixelTexture(uint32 color, const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        Verify(device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_resource)));

        Verify(device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_resource.Get(), 0, 1)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resourceUpload)));

        uint32 pixelData = color;
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = &pixelData;
        subresourceData.RowPitch = sizeof(uint32);
        subresourceData.SlicePitch = subresourceData.RowPitch;

        UpdateSubresources(
            device->GetCommandList().Get(),
            m_resource.Get(),
            m_resourceUpload.Get(),
            0,
            0,
            1,
            &subresourceData);

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        device->GetCommandList()->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = descriptorHeap->GetCPUHandle(descriptorIndex);
        device->GetDevice()->CreateShaderResourceView(m_resource.Get(), &srvDesc, srvHandle);

        m_descriptorIndex = descriptorIndex;
    }

    void Texture::CreateDefaultAlbedo(const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        CreateSinglePixelTexture(0xFFFFFFFF, device, descriptorHeap, descriptorIndex); // white
    }

    void Texture::CreateDefaultMetallic(const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        CreateSinglePixelTexture(0x00000000, device, descriptorHeap, descriptorIndex); // black
    }

    void Texture::CreateDefaultRoughness(const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        CreateSinglePixelTexture(0xFFFFFFFF, device, descriptorHeap, descriptorIndex); // white
    }

    void Texture::CreateDefaultNormal(const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        CreateSinglePixelTexture(0x8080FFFF, device, descriptorHeap, descriptorIndex); // (128, 128, 255)
    }
}
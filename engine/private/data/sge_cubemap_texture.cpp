#include "data/sge_cubemap_texture.h"

#include <filesystem>
using namespace DirectX;

namespace SGE
{
    void CubemapTexture::Initialize(const CubemapAssetData& assetData, const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        std::array<std::string, 6> texturePaths = assetData.GetPaths();
        CreateCubeMapTexture(texturePaths, device, descriptorHeap, descriptorIndex);
    }

    void CubemapTexture::CreateCubeMapTexture(const std::array<std::string, 6>& texturePaths, const Device* device, const DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        std::array<ScratchImage, 6> scratchImages;
        TexMetadata metadata;

        for (uint32 i = 0; i < 6; ++i)
        {
            if (!std::filesystem::exists(texturePaths[i]))
            {
                throw std::runtime_error("Texture file does not exist: " + texturePaths[i]);
            }

            std::filesystem::path filePath = texturePaths[i];
            std::wstring fileName = filePath.wstring();

            if (filePath.extension() == L".dds")
            {
                Verify(LoadFromDDSFile(fileName.c_str(), DDS_FLAGS_NONE, &metadata, scratchImages[i]));
            }
            else if (filePath.extension() == L".hdr")
            {
                Verify(LoadFromHDRFile(fileName.c_str(), &metadata, scratchImages[i]));
            }
            else if (filePath.extension() == L".tga")
            {
                Verify(LoadFromTGAFile(fileName.c_str(), &metadata, scratchImages[i]));
            }
            else
            {
                Verify(LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImages[i]));
            }
        }

        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureDesc.Alignment = 0;
        textureDesc.Width = metadata.width;
        textureDesc.Height = static_cast<uint32>(metadata.height);
        textureDesc.DepthOrArraySize = 6;
        textureDesc.MipLevels = static_cast<uint16>(metadata.mipLevels);
        textureDesc.Format = metadata.format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        Verify(device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_resource)));

        const uint32 subresourceCount = textureDesc.DepthOrArraySize * textureDesc.MipLevels;
        const uint64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, subresourceCount);

        Verify(device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resourceUpload)));

        ID3D12GraphicsCommandList* commandList = device->GetCommandList().Get();

        std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(subresourceCount);

        for (uint32 face = 0; face < 6; ++face)
        {
            for (uint32 mip = 0; mip < metadata.mipLevels; ++mip)
            {
                auto mipmap = scratchImages[face].GetImage(mip, 0, 0);
                uint32 subresourceIndex = D3D12CalcSubresource(mip, face, 0, static_cast<uint32>(metadata.mipLevels), 6);
                subresourceData[subresourceIndex].pData = mipmap->pixels;
                subresourceData[subresourceIndex].RowPitch = mipmap->rowPitch;
                subresourceData[subresourceIndex].SlicePitch = mipmap->slicePitch;
            }
        }

        UpdateSubresources(commandList, m_resource.Get(), m_resourceUpload.Get(), 0, 0, subresourceCount, subresourceData.data());

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = metadata.format == DXGI_FORMAT_UNKNOWN ? DXGI_FORMAT_R8G8B8A8_UNORM : metadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.TextureCube.MipLevels = static_cast<uint32>(metadata.mipLevels);
        srvDesc.TextureCube.MostDetailedMip = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = descriptorHeap->GetCPUHandle(descriptorIndex);
        device->GetDevice()->CreateShaderResourceView(m_resource.Get(), &srvDesc, srvHandle);

        m_descriptorIndex = descriptorIndex;
    }
}
#include "rendering/sge_render_target_texture.h"

#include "core/sge_helpers.h"

namespace SGE
{
    void RenderTargetTexture::Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height, DXGI_FORMAT format, uint32 index)
    {
        m_index = index;
        m_device = device;
        m_rtvHeap = rtvHeap;
        m_srvHeap = srvHeap;
        m_format = format;
        CreateTexture(width, height);
    }

    void RenderTargetTexture::Resize(uint32 width, uint32 height)
    {
        CreateTexture(width, height);
    }

    void RenderTargetTexture::Shutdown()
    {
        m_renderTarget.reset();
    }

    void RenderTargetTexture::CreateTexture(uint32 width, uint32 height)
    {
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = m_format;
        texDesc.SampleDesc.Count = 1;
        texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = m_format;
        memcpy(clearValue.Color, CLEAR_COLOR, sizeof(float) * 4);

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        ComPtr<ID3D12Resource> renderTarget;
        HRESULT hr = m_device->GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&renderTarget)
        );
        Verify(hr, "Failed to create render target texture.");

        m_renderTarget = std::make_unique<Resource>(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

        // Create RTV
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(RTT_START_RTV_HEAP_INDEX + m_index);
        m_device->GetDevice()->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);

        // Create SRV
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvHeap->GetCPUHandle(RTT_START_SRV_HEAP_INDEX + m_index);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = m_format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        m_device->GetDevice()->CreateShaderResourceView(renderTarget.Get(), &srvDesc, srvHandle);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::GetRTVHandle() const
    {
        return m_rtvHeap->GetCPUHandle(RTT_START_RTV_HEAP_INDEX + m_index);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::GetSRVHandle() const
    {
        return m_srvHeap->GetCPUHandle(RTT_START_SRV_HEAP_INDEX + m_index);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE RenderTargetTexture::GetSRVGPUHandle() const
    {
        return m_srvHeap->GetGPUHandle(RTT_START_SRV_HEAP_INDEX + m_index);
    }
}
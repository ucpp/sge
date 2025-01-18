#include "sge_gbuffer.h"
#include "sge_helpers.h"

namespace SGE
{
    void GBuffer::Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height)
    {
        m_device = device;
        m_rtvHeap = rtvHeap;
        m_srvHeap = srvHeap;

        CreateRenderTargets(width, height);
    }

    void GBuffer::Resize(uint32 width, uint32 height)
    {
        CreateRenderTargets(width, height);
    }

    void GBuffer::Shutdown()
    {
        m_renderTargets.clear();
    }

    void GBuffer::CreateRenderTargets(uint32 width, uint32 height)
    {
        DXGI_FORMAT formats[] = 
        {
            DXGI_FORMAT_R8G8B8A8_UNORM, // Albedo + Metallic
            DXGI_FORMAT_R10G10B10A2_UNORM   // Normal + Roughness
        };

        D3D12_CLEAR_VALUE clearValues[2] = {};

        // Albedo + Metallic (DXGI_FORMAT_R8G8B8A8_UNORM)
        clearValues[0].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValues[0].Color[0] = 0.0f; // Albedo (R)
        clearValues[0].Color[1] = 0.0f; // Albedo (G)
        clearValues[0].Color[2] = 0.0f; // Albedo (B)
        clearValues[0].Color[3] = 1.0f; // Metallic (A)

        // Normal + Roughness (DXGI_FORMAT_R10G10B10A2_UNORM)
        clearValues[1].Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        clearValues[1].Color[0] = 0.0f; // Normal X
        clearValues[1].Color[1] = 0.0f; // Normal Y
        clearValues[1].Color[2] = 0.0f; // Normal Z
        clearValues[1].Color[3] = 1.0f; // Roughness

        m_renderTargets.clear();
        for (uint32 i = 0; i < _countof(formats); ++i)
        {
            D3D12_RESOURCE_DESC texDesc = {};
            texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            texDesc.Width = width;
            texDesc.Height = height;
            texDesc.DepthOrArraySize = 1;
            texDesc.MipLevels = 1;
            texDesc.Format = formats[i];
            texDesc.SampleDesc.Count = 1;
            texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

            ComPtr<ID3D12Resource> renderTarget;
            HRESULT hr = m_device->GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &texDesc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                &clearValues[i],
                IID_PPV_ARGS(&renderTarget)
            );
            Verify(hr, "Failed to create GBuffer render target.");

            m_renderTargets.emplace_back(std::make_unique<Resource>(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET));

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(GBUFFER_START_RTV_HEAP_INDEX + static_cast<uint32>(m_renderTargets.size() - 1));
            m_device->GetDevice()->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);

            // Create SRV for the same resource
            CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvHeap->GetCPUHandle(GBUFFER_START_SRV_HEAP_INDEX + static_cast<uint32>(m_renderTargets.size() - 1));
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = formats[i];
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            m_device->GetDevice()->CreateShaderResourceView(renderTarget.Get(), &srvDesc, srvHandle);
        }
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GetRTVHandle(uint32 index) const
    {
        return m_rtvHeap->GetCPUHandle(GBUFFER_START_RTV_HEAP_INDEX + index);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GetSRVHandle(uint32 index) const
    {
        return m_srvHeap->GetCPUHandle(GBUFFER_START_SRV_HEAP_INDEX + index);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GetSRVGPUHandle(uint32 index) const
    {
        return m_srvHeap->GetGPUHandle(GBUFFER_START_SRV_HEAP_INDEX + index);
    }
}

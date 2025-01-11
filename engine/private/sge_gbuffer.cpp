#include "sge_gbuffer.h"
#include "sge_helpers.h"

namespace SGE
{
    void GBuffer::Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height, ID3D12GraphicsCommandList* commandList)
    {
        m_device = device;
        m_rtvHeap = rtvHeap;
        m_srvHeap = srvHeap;
        m_commandList = commandList;

        CreateRenderTargets(width, height, commandList);
    }

    void GBuffer::Resize(uint32 width, uint32 height)
    {
        Verify(m_commandList, "GBuffer::Resize: CommandList is invalid.");
        Shutdown();
        CreateRenderTargets(width, height, m_commandList);
    }

    void GBuffer::Shutdown()
    {
        for (auto& target : m_renderTargets)
        {
            if (target)
            {
                target.Reset();
            }
        }
        m_renderTargets.clear();
    }

    void GBuffer::CreateRenderTargets(uint32 width, uint32 height, ID3D12GraphicsCommandList* commandList)
    {
        DXGI_FORMAT formats[] = {
            DXGI_FORMAT_R16G16B16A16_FLOAT, // Albedo + Metallic
            DXGI_FORMAT_R10G10B10A2_UNORM, // Normal + Roughness
            DXGI_FORMAT_R32_FLOAT          // Depth
        };

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
                nullptr,
                IID_PPV_ARGS(&renderTarget)
            );
            Verify(hr, "Failed to create GBuffer render target.");

            m_renderTargets.push_back(renderTarget);

            const uint32 startIndex = Device::BufferCount * 2;
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(startIndex + static_cast<uint32>(m_renderTargets.size() - 1));
            m_device->GetDevice()->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = renderTarget.Get();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            commandList->ResourceBarrier(1, &barrier);

            // Create SRV for the same resource
            const uint32 startSrvIndex = Device::CbvSrvHeapCapacity - 10;
            CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvHeap->GetCPUHandle(startSrvIndex + static_cast<uint32>(m_renderTargets.size() - 1));
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
        const uint32 startIndex = Device::BufferCount * 2;
        return m_rtvHeap->GetCPUHandle(startIndex + index);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GetSRVHandle(uint32 index) const
    {
        const uint32 startIndex = Device::CbvSrvHeapCapacity - 10;
        return m_srvHeap->GetCPUHandle(startIndex + index);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GetSRVGPUHandle(uint32 index) const
    {
        const uint32 startIndex = Device::CbvSrvHeapCapacity - 10;
        return m_srvHeap->GetGPUHandle(startIndex + index);
    }
}

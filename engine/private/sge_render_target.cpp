#include "sge_render_target.h"

#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void RenderTarget::Initialize(Device* device, DescriptorHeap* rtvHeap, uint32 bufferCount, bool isMSAAEnabled)
    {
        if (!device)
        {
            LOG_ERROR("Device is null in RenderTarget::Initialize.");
            return;
        }

        if (!rtvHeap)
        {
            LOG_ERROR("DescriptorHeap is null in RenderTarget::Initialize.");
            return;
        }

        m_device = device;
        m_rtvHeap = rtvHeap;
        m_bufferCount = bufferCount;
        m_isMSAAEnabled = isMSAAEnabled;

        CreateRenderTargets();
    }

    void RenderTarget::CreateRenderTargets()
    {
        m_normalTargets.resize(m_bufferCount);
        m_msaaTargets.resize(m_bufferCount);

        for (uint32 i = 0; i < m_bufferCount; i++)
        {
            HRESULT hr = m_device->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_normalTargets[i]));
            Verify(hr, "Failed to get buffer from SwapChain.");

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(i);
            m_device->GetDevice()->CreateRenderTargetView(m_normalTargets[i].Get(), nullptr, rtvHandle);

            if (m_isMSAAEnabled)
            {
                DXGI_SWAP_CHAIN_DESC swapChainDesc;
                m_device->GetSwapChain()->GetDesc(&swapChainDesc);

                D3D12_RESOURCE_DESC texDesc = {};
                texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                texDesc.Alignment = 0;
                texDesc.Width = swapChainDesc.BufferDesc.Width;
                texDesc.Height = swapChainDesc.BufferDesc.Height;
                texDesc.Format = swapChainDesc.BufferDesc.Format;
                texDesc.DepthOrArraySize = 1;
                texDesc.SampleDesc.Count = 4;
                texDesc.SampleDesc.Quality = 0;
                texDesc.MipLevels = 1;
                texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

                CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

                hr = m_device->GetDevice()->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &texDesc,
                    D3D12_RESOURCE_STATE_PRESENT,
                    nullptr,
                    IID_PPV_ARGS(&m_msaaTargets[i]));
                Verify(hr, "Failed to create MSAA render target.");

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvMSAAHandle = m_rtvHeap->GetCPUHandle(i + BufferCount);
                m_device->GetDevice()->CreateRenderTargetView(m_msaaTargets[i].Get(), nullptr, rtvMSAAHandle); 
            }
        }
    }

    void RenderTarget::Shutdown()
    {
        for (auto& normalTarget : m_normalTargets)
        {
            if (normalTarget)
            {
                normalTarget.Reset();
            }
        }

        m_normalTargets.clear();

        for (auto& msaaTarget : m_msaaTargets)
        {
            if (msaaTarget)
            {
                msaaTarget.Reset();
            }
        }

        m_msaaTargets.clear();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(uint32 index, bool isMSAA) const
    {
        if (m_rtvHeap)
        {
            if (isMSAA)
            {
                return m_rtvHeap->GetCPUHandle(index + BufferCount);
            }
            else
            {
                return m_rtvHeap->GetCPUHandle(index);
            }
        }

        return {};
    }

    void RenderTarget::Resize(uint32 width, uint32 height)
    {
        if (!m_device)
        {
            LOG_ERROR("Device is null in RenderTarget::Resize.");
            return;
        }

        if (!m_rtvHeap)
        {
            LOG_ERROR("DescriptorHeap is null in RenderTarget::Resize.");
            return;
        }

        Shutdown();
        CreateRenderTargets();
    }

    void RenderTarget::Resolve(ID3D12GraphicsCommandList* commandList, uint32 index)
    {
        if (!m_isMSAAEnabled)
        {
            return;
        }

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_msaaTargets[index].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        commandList->ResourceBarrier(1, &barrier);

        commandList->ResolveSubresource(
            m_normalTargets[index].Get(),
            0,
            m_msaaTargets[index].Get(),
            0,
            DXGI_FORMAT_R8G8B8A8_UNORM);

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_msaaTargets[index].Get(),
            D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &barrier);
    }
}
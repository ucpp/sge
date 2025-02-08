#include "rendering/sge_render_target.h"

#include "core/sge_descriptor_heap.h"
#include "core/sge_device.h"
#include "core/sge_helpers.h"
#include "core/sge_logger.h"

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
        m_normalTargets.clear();
        m_msaaTargets.clear();
        m_normalTargets.resize(m_bufferCount);
        m_msaaTargets.resize(m_bufferCount);

        for (uint32 i = 0; i < m_bufferCount; i++)
        {
            ComPtr<ID3D12Resource> rtResource;
            HRESULT hr = m_device->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&rtResource));
            Verify(hr, "Failed to get buffer from SwapChain.");
            m_normalTargets[i] = std::make_unique<Resource>(rtResource.Get(), D3D12_RESOURCE_STATE_COMMON);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(i);
            m_device->GetDevice()->CreateRenderTargetView(m_normalTargets[i]->Get(), nullptr, rtvHandle);
            m_normalTargets[i]->Get()->SetName(L"Normal Render Target");

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

                D3D12_CLEAR_VALUE clearValue = {};
                clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                clearValue.Color[0] = 0.0f; // R
                clearValue.Color[1] = 0.0f; // G
                clearValue.Color[2] = 0.0f; // B
                clearValue.Color[3] = 1.0f; // A

                CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
                ComPtr<ID3D12Resource> msaaResource;
                hr = m_device->GetDevice()->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &texDesc,
                    D3D12_RESOURCE_STATE_PRESENT,
                    &clearValue,
                    IID_PPV_ARGS(&msaaResource));
                Verify(hr, "Failed to create MSAA render target.");
                m_msaaTargets[i] = std::make_unique<Resource>(msaaResource.Get(), D3D12_RESOURCE_STATE_PRESENT);
                m_msaaTargets[i]->Get()->SetName(L"Normal Render Target");

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvMSAAHandle = m_rtvHeap->GetCPUHandle(i + BUFFER_COUNT);
                m_device->GetDevice()->CreateRenderTargetView(m_msaaTargets[i]->Get(), nullptr, rtvMSAAHandle); 
            }
        }
    }

    void RenderTarget::Shutdown()
    {
        m_normalTargets.clear();
        m_msaaTargets.clear();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(uint32 index, bool isMSAA) const
    {
        if (m_rtvHeap)
        {
            uint32 resolvedIndex = isMSAA ? index + BUFFER_COUNT : index;
            return m_rtvHeap->GetCPUHandle(resolvedIndex);
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

        CreateRenderTargets();
    }

    void RenderTarget::Resolve(ID3D12GraphicsCommandList* commandList, uint32 index)
    {
        if (!m_isMSAAEnabled)
        {
            return;
        }
 
        m_msaaTargets[index]->TransitionState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE, commandList);
        commandList->ResolveSubresource(m_normalTargets[index]->Get(), 0, m_msaaTargets[index]->Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
        m_msaaTargets[index]->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
    }
}
#include "sge_render_target.h"

#include "sge_device.h"
#include "sge_helpers.h"

namespace SGE
{
    void RenderTarget::Initialize(Device* device, UINT bufferCount)
    {
        m_rtvDescriptorSize = device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = bufferCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        HRESULT hr = device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));
        Verify(hr, "Failed to create RTV descriptor heap.");

        m_renderTargets.resize(bufferCount);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < bufferCount; i++)
        {
            hr = device->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            Verify(hr, "Failed to get buffer from SwapChain.");
           
            device->GetDevice()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    void RenderTarget::Shutdown()
    {
        m_renderTargets.clear();
        m_rtvHeap.Reset();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(UINT index) const
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
        handle.Offset(index, m_rtvDescriptorSize);
        return handle;
    }
}

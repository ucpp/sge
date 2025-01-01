#include "sge_render_target.h"

#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void RenderTarget::Initialize(Device* device, DescriptorHeap* rtvHeap, uint32 bufferCount)
    {
        if (rtvHeap == nullptr)
        {
            LOG_ERROR("DescriptorHeap is null in RenderTarget::Initialize.");
            return;
        }

        m_rtvHeap = rtvHeap;

        m_renderTargets.resize(bufferCount);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(0);

        for (uint32 i = 0; i < bufferCount; i++)
        {
            HRESULT hr = device->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            Verify(hr, "Failed to get buffer from SwapChain.");

            device->GetDevice()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvHeap->GetDescriptorSize());
        }
    }

    void RenderTarget::Shutdown()
    {
        m_renderTargets.clear();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(uint32 index) const
    {
        if (m_rtvHeap)
        {
            return m_rtvHeap->GetCPUHandle(index);
        }

        return {};
    }
}

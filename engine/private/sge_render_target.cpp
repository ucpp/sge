#include "sge_render_target.h"

#include "sge_descriptor_heap.h"
#include "sge_device.h"
#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void RenderTarget::Initialize(Device* device, DescriptorHeap* rtvHeap, uint32 bufferCount)
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

        CreateRenderTargets();
    }

    void RenderTarget::CreateRenderTargets()
    {
        m_renderTargets.resize(m_bufferCount);

        for (uint32 i = 0; i < m_bufferCount; i++)
        {
            HRESULT hr = m_device->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            Verify(hr, "Failed to get buffer from SwapChain.");

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandle(i);
            m_device->GetDevice()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        }
    }

    void RenderTarget::Shutdown()
    {
        for (auto& renderTarget : m_renderTargets)
        {
            if (renderTarget)
            {
                renderTarget.Reset();
            }
        }

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
}

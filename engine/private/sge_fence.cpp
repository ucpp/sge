#include "sge_fence.h"

#include "sge_device.h"
#include "sge_helpers.h"

namespace SGE
{
    Fence::~Fence()
    {
        if (m_fenceEvent)
        {
            CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }
    }

    void Fence::Initialize(Device* device, UINT64 initialValue)
    {
        HRESULT hr = device->GetDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        Verify(hr, "Failed to create D3D12 fence.");

        m_fenceValue = initialValue;
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        Verify(m_fenceEvent, "Failed to create event handle.");
    }

    UINT64 Fence::Signal(ID3D12CommandQueue* commandQueue)
    {
        const UINT64 fence = m_fenceValue;
        HRESULT hr = commandQueue->Signal(m_fence.Get(), fence);
        m_fenceValue++;
        Verify(hr, "Failed to signal command queue.");

        return fence;
    }

    void Fence::Wait(UINT64 waitValue)
    {
        if (m_fence->GetCompletedValue() < waitValue)
        {
            HRESULT hr = m_fence->SetEventOnCompletion(waitValue, m_fenceEvent);
            Verify(hr, "Failed to set event on fence completion.");
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    UINT64 Fence::GetCompletedValue() const
    {
        return m_fence->GetCompletedValue();
    }
}
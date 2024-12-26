#ifndef _SGE_FENCE_H_
#define _SGE_FENCE_H_

#include "sge_device.h"

namespace SGE
{
    class Fence final
    {
    public:
        Fence() = default;
        ~Fence();

        void Initialize(Device* device, UINT64 initialValue = 0);
        UINT64 Signal(ID3D12CommandQueue* commandQueue);
        void Wait(UINT64 waitValue);

        UINT64 GetCompletedValue() const;
        UINT64 GetCurrentFenceValue() const { return m_fenceValue; }

    private:
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValue = 0;
        HANDLE m_fenceEvent = nullptr;
    };
}

#endif // !_SGE_FENCE_H_
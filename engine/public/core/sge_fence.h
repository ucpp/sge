#ifndef _SGE_FENCE_H_
#define _SGE_FENCE_H_

#include "pch.h"

namespace SGE
{
    class Fence final
    {
    public:
        Fence() = default;
        ~Fence();

        void Initialize(class Device* device, uint64 initialValue = 0);
        uint64 Signal(ID3D12CommandQueue* commandQueue);
        void Wait(uint64 waitValue);

        uint64 GetCompletedValue() const;
        uint64 GetCurrentFenceValue() const { return m_fenceValue; }

    private:
        ComPtr<ID3D12Fence> m_fence;
        uint64 m_fenceValue = 0;
        HANDLE m_fenceEvent = nullptr;
    };
}

#endif // !_SGE_FENCE_H_
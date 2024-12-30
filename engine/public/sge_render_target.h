#ifndef _SGE_RENDER_TARGET_H_
#define _SGE_RENDER_TARGET_H_

#include "pch.h"

namespace SGE
{
    class RenderTarget
    {
    public:
        void Initialize(class Device* device, UINT bufferCount);
        void Shutdown();

        ID3D12Resource* GetTarget(UINT index) const { return m_renderTargets[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(UINT index) const;

    private:
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
        UINT m_rtvDescriptorSize = 0;
    };
}

#endif // !_SGE_RENDER_TARGET_H_

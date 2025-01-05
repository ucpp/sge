#ifndef _SGE_RENDER_TARGET_H_
#define _SGE_RENDER_TARGET_H_

#include "pch.h"

namespace SGE
{
    class RenderTarget
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* rtvHeap, uint32 bufferCount);
        void Resize(uint32 width, uint32 height);
        void Shutdown();

        ID3D12Resource* GetTarget(uint32 index) const { return m_renderTargets[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32 index) const;

    private:
        void CreateRenderTargets();

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_rtvHeap = nullptr;
        std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
        uint32 m_bufferCount = 0;  
    };
}

#endif // !_SGE_RENDER_TARGET_H_

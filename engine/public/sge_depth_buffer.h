#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* dsvHeap, uint32 width, uint32 height, uint32 bufferCount);
        void Shutdown();
        ID3D12Resource* GetDepthBuffer(uint32 index) const { return m_depthBuffers[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32 index) const;

    private:
        std::vector<ComPtr<ID3D12Resource>> m_depthBuffers;
        class DescriptorHeap* m_dsvHeap = nullptr;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

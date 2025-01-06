#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* dsvHeap, uint32 width, uint32 height, uint32 bufferCount, bool isMSAAEnabled = false);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        ID3D12Resource* GetDepthBuffer(uint32 index) const { return m_depthBuffers[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32 index) const;

    private:
        void CreateDepthBuffer(Device* device, uint32 width, uint32 height, uint32 index);

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_dsvHeap = nullptr;
        std::vector<ComPtr<ID3D12Resource>> m_depthBuffers;
        uint32 m_bufferCount;
        bool m_isMSAAEnabled = false;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

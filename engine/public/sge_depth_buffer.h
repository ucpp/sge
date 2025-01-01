#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"
#include "sge_device.h"
#include "sge_helpers.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(Device* device, uint32 width, uint32 height, uint32 bufferCount);
        void Shutdown();
        ID3D12Resource* GetDepthBuffer(uint32 index) const { return m_depthBuffers[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32 index) const;

    private:
        std::vector<ComPtr<ID3D12Resource>> m_depthBuffers;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> m_dsvHandles;
        uint32 m_dsvDescriptorSize = 0;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

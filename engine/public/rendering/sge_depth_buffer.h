#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"
#include "core/sge_resource.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* dsvHeap, class DescriptorHeap* srvHeap, uint32 width, uint32 height, uint32 descriptorIndex = 0);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        Resource* GetResource() const { return m_depthBuffer.get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;

    private:
        void CreateDepthBuffer(Device* device, uint32 width, uint32 height);

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_dsvHeap = nullptr;
        class DescriptorHeap* m_srvHeap = nullptr;
        std::unique_ptr<Resource> m_depthBuffer;
        uint32 m_descriptorIndex = 0;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

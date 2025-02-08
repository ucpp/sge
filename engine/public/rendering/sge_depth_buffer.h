#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"
#include "core/sge_resource.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* dsvHeap, class DescriptorHeap* srvHeap, uint32 width, uint32 height, uint32 bufferCount, bool isMSAAEnabled = false);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        Resource* GetResource(uint32 index) const { return m_depthBuffers[index].get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32 index) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(uint32 index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(uint32 index) const;

    private:
        void CreateDepthBuffer(Device* device, uint32 width, uint32 height, uint32 index);

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_dsvHeap = nullptr;
        class DescriptorHeap* m_srvHeap = nullptr;
        std::vector<std::unique_ptr<Resource>> m_depthBuffers;

        uint32 m_bufferCount;
        bool m_isMSAAEnabled = false;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

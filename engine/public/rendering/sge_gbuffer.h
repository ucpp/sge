#ifndef _SGE_GBUFFER_H_
#define _SGE_GBUFFER_H_

#include "pch.h"
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_resource.h"

namespace SGE
{
    class GBuffer
    {
    public:
        void Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height);
        void Resize(uint32 width, uint32 height);
        void Shutdown();

        Resource* GetResource(uint32 index) const { return m_renderTargets[index].get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32 index) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(uint32 index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(uint32 index) const;
        
        uint32 GetTargetCount() const { return static_cast<uint32>(m_renderTargets.size()); }

    private:
        void CreateRenderTargets(uint32 width, uint32 height);

    private:
        Device* m_device = nullptr;
        DescriptorHeap* m_rtvHeap = nullptr;
        DescriptorHeap* m_srvHeap = nullptr;

        std::vector<std::unique_ptr<Resource>> m_renderTargets;
        std::vector<CD3DX12_GPU_DESCRIPTOR_HANDLE> m_srvGpuHandles;
    };
}

#endif // !_SGE_GBUFFER_H_

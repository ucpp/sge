#ifndef _SGE_RENDER_TARGET_TEXTURE_H_
#define _SGE_RENDER_TARGET_TEXTURE_H_

#include "pch.h"
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_resource.h"
#include "data/sge_data_structures.h"

namespace SGE
{
    class RenderTargetTexture
    {
    public:
        void Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height, DXGI_FORMAT format, uint32 index);

        void Resize(uint32 width, uint32 height);
        void Shutdown();

        Resource* GetResource() const { return m_renderTarget.get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;

    private:
        void CreateTexture(uint32 width, uint32 height);

    private:
        Device* m_device = nullptr;
        DescriptorHeap* m_rtvHeap = nullptr;
        DescriptorHeap* m_srvHeap = nullptr;

        std::unique_ptr<Resource> m_renderTarget;
        DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;

        uint32 m_index = 0;
    };
}

#endif // !_SGE_RENDER_TARGET_TEXTURE_H_
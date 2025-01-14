#ifndef _SGE_RENDER_TARGET_H_
#define _SGE_RENDER_TARGET_H_

#include "pch.h"
#include "sge_resource.h"

namespace SGE
{
    class RenderTarget
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* rtvHeap, uint32 bufferCount, bool isMSAAEnabled = false);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        void Resolve(ID3D12GraphicsCommandList* commandList, uint32 index);

        Resource* GetResource(uint32 index, bool isMSAA = false) const { return isMSAA ? m_msaaTargets[index].get(): m_normalTargets[index].get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32 index, bool isMSAA) const;

    private:
        void CreateRenderTargets();

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_rtvHeap = nullptr;
        std::vector<std::unique_ptr<Resource>> m_normalTargets;
        std::vector<std::unique_ptr<Resource>> m_msaaTargets;

        uint32 m_bufferCount = 0;
        bool m_isMSAAEnabled = false;
    };
}

#endif // !_SGE_RENDER_TARGET_H_

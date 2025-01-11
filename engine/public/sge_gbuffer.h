#ifndef _SGE_GBUFFER_H_
#define _SGE_GBUFFER_H_

#include "pch.h"
#include "sge_device.h"
#include "sge_descriptor_heap.h"

namespace SGE
{
    class GBuffer
    {
    public:
        void Initialize(Device* device, DescriptorHeap* rtvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height, ID3D12GraphicsCommandList* commandList);
        void Resize(uint32 width, uint32 height);
        void Shutdown();

        ID3D12Resource* GetRenderTarget(uint32 index) const { return m_renderTargets[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32 index) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(uint32 index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(uint32 index) const;

        uint32 GetTargetCount() const { return static_cast<uint32>(m_renderTargets.size()); }

    private:
        void CreateRenderTargets(uint32 width, uint32 height, ID3D12GraphicsCommandList* commandList);

    private:
        Device* m_device = nullptr;
        DescriptorHeap* m_rtvHeap = nullptr;
        DescriptorHeap* m_srvHeap = nullptr;
        ID3D12GraphicsCommandList* m_commandList = nullptr;
        std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
        std::vector<CD3DX12_GPU_DESCRIPTOR_HANDLE> m_srvGpuHandles;
    };
}

#endif // !_SGE_GBUFFER_H_

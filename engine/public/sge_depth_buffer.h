#ifndef _SGE_DEPTH_BUFFER_H_
#define _SGE_DEPTH_BUFFER_H_

#include "pch.h"

namespace SGE
{
    class DepthBuffer
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* dsvHeap, class DescriptorHeap* srvHeap, uint32 width, uint32 height, uint32 bufferCount, bool isMSAAEnabled = false);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        ID3D12Resource* GetDepthBuffer(uint32 index) const { return m_depthBuffers[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32 index) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(uint32 index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(uint32 index) const;

        D3D12_RESOURCE_STATES GetCurrentState(uint32 index) const;
        void SetCurrentState(D3D12_RESOURCE_STATES state, uint32 index);

    private:
        void CreateDepthBuffer(Device* device, uint32 width, uint32 height, uint32 index);

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_dsvHeap = nullptr;
        class DescriptorHeap* m_srvHeap = nullptr;
        std::vector<ComPtr<ID3D12Resource>> m_depthBuffers;
        std::vector<D3D12_RESOURCE_STATES> m_states;
        uint32 m_bufferCount;
        bool m_isMSAAEnabled = false;
    };
}

#endif // !_SGE_DEPTH_BUFFER_H_

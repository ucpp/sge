#ifndef _SGE_DESCRIPTOR_HEAP_H_
#define _SGE_DESCRIPTOR_HEAP_H_

#include "pch.h"

namespace SGE
{
    class DescriptorHeap
    {
    public:
        void Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors, bool shaderVisible = false);
        void Shutdown();

        ComPtr<ID3D12DescriptorHeap> GetHeap() const { return m_heap; }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32 index) const;
        uint32 GetDescriptorSize() const { return m_descriptorSize; }

    private:
        ComPtr<ID3D12DescriptorHeap> m_heap;
        uint32 m_descriptorSize = 0;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuHandle = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = {};
    };
}

#endif // !_SGE_DESCRIPTOR_HEAP_H_
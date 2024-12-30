#ifndef _SGE_DESCRIPTOR_HEAP_H_
#define _SGE_DESCRIPTOR_HEAP_H_

#include "pch.h"

namespace SGE
{
    class DescriptorHeap
    {
    public:
        void Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible = false);
        void Shutdown();

        ComPtr<ID3D12DescriptorHeap> GetHeap() const { return m_heap; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const;
        UINT GetDescriptorSize() const { return m_descriptorSize; }

    private:
        ComPtr<ID3D12DescriptorHeap> m_heap;
        UINT m_descriptorSize = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = {};
    };
}

#endif // !_SGE_DESCRIPTOR_HEAP_H_
#ifndef _SGE_CONSTANT_BUFFER_H_
#define _SGE_CONSTANT_BUFFER_H_

#include "pch.h"
#include "core/sge_descriptor_heap.h"

namespace SGE
{
    class ConstantBuffer
    {
    public:
        void Initialize(ID3D12Device* device, DescriptorHeap* descriptorHeap, size_t bufferSize, uint32 descriptorIndex);
        void Update(const void* data, size_t dataSize);
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() const { return m_gpuDescriptorHandle; }

    private:
        ComPtr<ID3D12Resource> m_buffer;
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptorHandle = {};
        DescriptorHeap* m_descriptorHeap = nullptr;
    };
}

#endif // _SGE_CONSTANT_BUFFER_H_

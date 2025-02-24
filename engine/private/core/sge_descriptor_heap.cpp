#include "core/sge_descriptor_heap.h"

#include "core/sge_logger.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void DescriptorHeap::Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors, bool shaderVisible)
    {
        if (!device || numDescriptors == 0)
        {
            LOG_ERROR("Invalid parameters for DescriptorHeap::Initialize");
            return;
        }

        if (m_heap)
        {
            LOG_WARN("DescriptorHeap is already initialized.");
            return;
        }

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = type;
        desc.NumDescriptors = numDescriptors;
        desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        Verify(hr, "Failed to create descriptor heap.");

        m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
        m_cpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();

        if (shaderVisible)
        {
            m_gpuHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
        }
        else
        {
            m_gpuHandle.ptr = 0;
        }
    }
    
    void DescriptorHeap::Shutdown()
    {
        if (!m_heap)
        {
            LOG_WARN("DescriptorHeap is not initialized or already shut down.");
            return;
        }

        m_heap.Reset();
        m_descriptorSize = 0;
        m_cpuHandle.ptr = 0;
        m_gpuHandle.ptr = 0;
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(uint32 index) const
    {
        if (!m_heap)
        {
            LOG_ERROR("DescriptorHeap is not initialized.");
            return {};
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle = m_cpuHandle;
        handle.ptr += index * m_descriptorSize;
        return handle;
    }
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(uint32 index) const
    {
        if (!m_heap.Get())
        {
            LOG_ERROR("DescriptorHeap is not initialized.");
            return {};
        }

        CD3DX12_GPU_DESCRIPTOR_HANDLE handle = m_gpuHandle;
        handle.ptr += index * m_descriptorSize;
        return handle;
    }
}
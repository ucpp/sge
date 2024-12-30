#include "sge_constant_buffer.h"

#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void ConstantBuffer::Initialize(ID3D12Device* device, DescriptorHeap* descriptorHeap, size_t bufferSize, UINT descriptorIndex)
    {
        m_descriptorHeap = descriptorHeap;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Alignment = 0;
        bufferDesc.Width = bufferSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.SampleDesc.Quality = 0;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer)
        );

        Verify(hr, "Failed to create constant buffer.");

        m_gpuDescriptorHandle = m_descriptorHeap->GetGPUHandle(descriptorIndex);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.SizeInBytes = (static_cast<UINT>(bufferSize) + 255) & ~255;
        cbvDesc.BufferLocation = m_buffer->GetGPUVirtualAddress();

        device->CreateConstantBufferView(&cbvDesc, m_descriptorHeap->GetCPUHandle(descriptorIndex));
    }

    void ConstantBuffer::Update(const void* data, size_t dataSize)
    {
        if (dataSize > m_buffer->GetDesc().Width)
        {
            throw std::runtime_error("Data size exceeds buffer size.");
        }

        void* mappedData = nullptr;
        HRESULT hr = m_buffer->Map(0, nullptr, &mappedData);
        Verify(hr, "Failed to map constant buffer.");

        memcpy(mappedData, data, dataSize);

        m_buffer->Unmap(0, nullptr);
    }
}

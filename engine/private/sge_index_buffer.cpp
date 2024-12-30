#include "sge_index_buffer.h"

#include "sge_device.h"

#include "sge_helpers.h"

namespace SGE
{
    void IndexBuffer::Initialize(Device* device, const std::vector<UINT>& indices)
    {
        m_indexCount = static_cast<UINT>(indices.size());
        const UINT indexBufferSize = static_cast<UINT>(indices.size() * sizeof(UINT));

        HRESULT hr = device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resource)
        );
        Verify(hr, "Failed to create index buffer resource.");

        UINT8* pIndexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        Verify(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)), "Failed to map index buffer.");

        memcpy(pIndexDataBegin, indices.data(), indexBufferSize);
        m_resource->Unmap(0, nullptr);

        m_view.BufferLocation = m_resource->GetGPUVirtualAddress();
        m_view.SizeInBytes = indexBufferSize;
        m_view.Format = DXGI_FORMAT_R32_UINT;
    }

    void IndexBuffer::Shutdown()
    {
        m_resource.Reset();
    }
}
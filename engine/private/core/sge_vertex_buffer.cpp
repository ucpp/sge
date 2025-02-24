#include "core/sge_vertex_buffer.h"

#include "core/sge_helpers.h"
#include "core/sge_device.h"
#include "data/sge_data_structures.h"

namespace SGE
{
    void VertexBuffer::Initialize(Device* device, const std::vector<Vertex>& vertices)
    {
        const UINT vertexBufferSize = static_cast<UINT>(vertices.size() * sizeof(Vertex));

        HRESULT hr = device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resource)
        );
        Verify(hr, "Failed to create vertex buffer resource.");

        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        Verify(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)), "Failed to map vertex buffer.");
        memcpy(pVertexDataBegin, vertices.data(), vertexBufferSize);
        m_resource->Unmap(0, nullptr);
        
        m_view = {};
        m_view.BufferLocation = m_resource->GetGPUVirtualAddress();
        m_view.StrideInBytes = sizeof(Vertex);
        m_view.SizeInBytes = vertexBufferSize;
    }

    void VertexBuffer::Shutdown()
    {
        m_resource.Reset();
    }
}
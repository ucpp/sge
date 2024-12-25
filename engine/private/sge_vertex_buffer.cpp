#include "sge_vertex_buffer.h"

#include "sge_helpers.h"
#include <d3dx12.h>

namespace SGE
{
    void VertexBuffer::Initialize(Device *device, ID3D12GraphicsCommandList *commandList, const std::vector<Vertex> &vertices)
    {
        ID3D12Device* d3dDevice = device->GetDevice().Get();
        size_t bufferSize = vertices.size() * sizeof(Vertex);

        ComPtr<ID3D12Resource> resource;
        HRESULT hr = d3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource));

        Verify(hr, "Failed to create vertex buffer resource.");
        m_resource = resource;

        ComPtr<ID3D12Resource> resourceUpload;
        hr = d3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resourceUpload));

        Verify(hr, "Failed to create vertex buffer upload resource.");

        m_resourceUpload = resourceUpload;

        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = vertices.data();
        vertexData.RowPitch = bufferSize;
        vertexData.SlicePitch = bufferSize;

        UpdateSubresources<1>(commandList, m_resource.Get(), m_resourceUpload.Get(), 0, 0, 1, &vertexData);
        
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            m_resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        m_view.BufferLocation = m_resource->GetGPUVirtualAddress();
        m_view.SizeInBytes = static_cast<UINT>(bufferSize);
        m_view.StrideInBytes = static_cast<UINT>(sizeof(Vertex));
    }
    
    void VertexBuffer::Shutdown()
    {
        m_resource.Reset();
        m_resourceUpload.Reset();
    }
    
    void VertexBuffer::ReleaseUploadResource()
    {
        m_resourceUpload.Reset();
    }
}
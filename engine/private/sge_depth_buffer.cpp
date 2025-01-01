#include "sge_depth_buffer.h"
#include "sge_device.h"
#include "sge_helpers.h"

namespace SGE
{
    void DepthBuffer::Initialize(Device* device, uint32 width, uint32 height, uint32 bufferCount)
    {
        m_dsvDescriptorSize = device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = bufferCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        HRESULT hr = device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap));
        Verify(hr, "Failed to create DSV descriptor heap.");

        m_depthBuffers.resize(bufferCount);
        m_dsvHandles.resize(bufferCount);

        for (uint32 i = 0; i < bufferCount; i++)
        {
            D3D12_RESOURCE_DESC depthDesc = {};
            depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            depthDesc.Alignment = 0;
            depthDesc.Width = width;
            depthDesc.Height = height;
            depthDesc.DepthOrArraySize = 1;
            depthDesc.MipLevels = 1;
            depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthDesc.SampleDesc.Count = 1;
            depthDesc.SampleDesc.Quality = 0;
            depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            hr = device->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &depthDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                nullptr,
                IID_PPV_ARGS(&m_depthBuffers[i])
            );
            Verify(hr, "Failed to create depth buffer.");

            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
            dsvHandle.Offset(i, m_dsvDescriptorSize);

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;

            device->GetDevice()->CreateDepthStencilView(m_depthBuffers[i].Get(), &dsvDesc, dsvHandle);
            m_dsvHandles[i] = dsvHandle;
        }
    }

    void DepthBuffer::Shutdown()
    {
        m_depthBuffers.clear();
        m_dsvHandles.clear();
        m_dsvHeap.Reset();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetDSVHandle(uint32 index) const
    {
        return m_dsvHandles[index];
    }
}

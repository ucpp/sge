#include "sge_depth_buffer.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void DepthBuffer::Initialize(Device* device, DescriptorHeap* dsvHeap, uint32 width, uint32 height, uint32 bufferCount)
    {
        if (dsvHeap == nullptr)
        {
            LOG_ERROR("DescriptorHeap is null in DepthBuffer::Initialize.");
            return;
        }

        m_dsvHeap = dsvHeap;
        m_depthBuffers.resize(bufferCount);

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

            HRESULT hr = device->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &depthDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                nullptr,
                IID_PPV_ARGS(&m_depthBuffers[i])
            );
            Verify(hr, "Failed to create depth buffer.");

            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUHandle(i);

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;

            device->GetDevice()->CreateDepthStencilView(m_depthBuffers[i].Get(), &dsvDesc, dsvHandle);
        }
    }

    void DepthBuffer::Shutdown()
    {
        m_depthBuffers.clear();
        m_dsvHeap = nullptr;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetDSVHandle(uint32 index) const
    {
        if (m_dsvHeap)
        {
            return m_dsvHeap->GetCPUHandle(index);
        }

        return {};
    }
}

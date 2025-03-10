#include "rendering/sge_depth_buffer.h"

#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "core/sge_helpers.h"
#include "core/sge_logger.h"

namespace SGE
{
    void DepthBuffer::Initialize(Device* device, DescriptorHeap* dsvHeap, DescriptorHeap* srvHeap, uint32 width, uint32 height, uint32 descriptorIndex)
    {
        if (!device)
        {
            LOG_ERROR("Device is null in DepthBuffer::Initialize.");
            return;
        }

        if (!dsvHeap)
        {
            LOG_ERROR("DSV DescriptorHeap is null in DepthBuffer::Initialize.");
            return;
        }

        if (!srvHeap)
        {
            LOG_ERROR("SRV DescriptorHeap is null in DepthBuffer::Initialize.");
            return;
        }

        m_dsvHeap = dsvHeap;
        m_srvHeap = srvHeap;
        m_device = device;
        m_descriptorIndex = descriptorIndex;

        CreateDepthBuffer(device, width, height);
    }

    void DepthBuffer::Shutdown()
    {
        m_depthBuffer.reset();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetDSVHandle() const
    {
        if (m_dsvHeap)
        {
            return m_dsvHeap->GetCPUHandle(m_descriptorIndex);
        }

        return {};
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetSRVHandle() const
    {
        return m_srvHeap->GetCPUHandle(DEPTH_BUFFER_START_SRV_HEAP_INDEX + m_descriptorIndex);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DepthBuffer::GetSRVGPUHandle() const
    {
        return m_srvHeap->GetGPUHandle(DEPTH_BUFFER_START_SRV_HEAP_INDEX + m_descriptorIndex);
    }

    void DepthBuffer::CreateDepthBuffer(Device* device, uint32 width, uint32 height)
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

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        ComPtr<ID3D12Resource> depthBufferResource;
        HRESULT hr = device->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &depthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue,
            IID_PPV_ARGS(&depthBufferResource)
        );
        Verify(hr, "Failed to create depth buffer.");
        m_depthBuffer = std::make_unique<Resource>(depthBufferResource.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUHandle(m_descriptorIndex);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        device->GetDevice()->CreateDepthStencilView(m_depthBuffer->Get(), &dsvDesc, dsvHandle);

        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvHeap->GetCPUHandle(DEPTH_BUFFER_START_SRV_HEAP_INDEX + m_descriptorIndex);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        device->GetDevice()->CreateShaderResourceView(m_depthBuffer->Get(), &srvDesc, srvHandle);
        m_depthBuffer->Get()->SetName(L"Depth Buffer");
    }

    void DepthBuffer::Resize(uint32 width, uint32 height)
    {
        if (!m_device)
        {
            LOG_ERROR("Device is null in DepthBuffer::Resize.");
            return;
        }

        if (!m_dsvHeap)
        {
            LOG_ERROR("DescriptorHeap is null in DepthBuffer::Resize.");
            return;
        }

        CreateDepthBuffer(m_device, width, height);
    }
}
#ifndef _SGE_INDEX_BUFFER_H_
#define _SGE_INDEX_BUFFER_H_

#include "pch.h"

namespace SGE
{
    class IndexBuffer
    {
    public:
        void Initialize(class Device* device, const std::vector<uint32>& indices);
        void Shutdown();

        D3D12_INDEX_BUFFER_VIEW GetView() const { return m_view; }
        ID3D12Resource* GetResource() const { return m_resource.Get(); }
        uint32 GetIndexCount() const { return m_indexCount; }

    private:
        ComPtr<ID3D12Resource> m_resource;
        D3D12_INDEX_BUFFER_VIEW m_view = {};
        uint32 m_indexCount = {};
    };
}

#endif // !_SGE_INDEX_BUFFER_H_

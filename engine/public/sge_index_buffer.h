#ifndef _SGE_INDEX_BUFFER_H_
#define _SGE_INDEX_BUFFER_H_

#include <vector>
#include <d3dx12.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "sge_device.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace SGE
{
    class IndexBuffer
    {
    public:
        void Initialize(Device* device, const std::vector<UINT>& indices);
        void Shutdown();

        D3D12_INDEX_BUFFER_VIEW GetView() const { return m_view; }
        ID3D12Resource* GetResource() const { return m_resource.Get(); }
        UINT GetIndexCount() const { return m_indexCount; }

    private:
        ComPtr<ID3D12Resource> m_resource;
        D3D12_INDEX_BUFFER_VIEW m_view = {};
        UINT m_indexCount = {};
    };
}

#endif // !_SGE_INDEX_BUFFER_H_

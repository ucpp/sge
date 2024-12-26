#ifndef _SGE_VERTEX_BUFFER_H_
#define _SGE_VERTEX_BUFFER_H_

#include <vector>
#include <DirectXMath.h>
#include <wrl.h>

#include "sge_device.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace SGE
{
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    class VertexBuffer
    {
    public:
        void Initialize(Device* device, const std::vector<Vertex>& vertices);
        void Shutdown();

        D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_view; }
        ID3D12Resource* GetResource() const { return m_resource.Get(); }

    private:
        ComPtr<ID3D12Resource> m_resource;
        D3D12_VERTEX_BUFFER_VIEW m_view = {};
    };
}

#endif // !_SGE_VERTEX_BUFFER_H
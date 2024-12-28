#ifndef _SGE_MESH_H_
#define _SGE_MESH_H_

#include <vector>
#include <d3d12.h>
#include "sge_vertex_buffer.h"
#include "sge_index_buffer.h"

namespace SGE
{
    class Mesh
    {
    public:
        void Initialize(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, Device* device);
        void Render(ID3D12GraphicsCommandList* commandList) const;

        const VertexBuffer& GetVertexBuffer() const { return m_vertexBuffer; }
        const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

    private:
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
    };
}

#endif // !_SGE_MESH_H_
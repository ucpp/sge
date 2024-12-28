#include "sge_mesh.h"

namespace SGE
{
    void Mesh::Initialize(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, Device* device)
    {
        m_vertexBuffer.Initialize(device, vertices);
        m_indexBuffer.Initialize(device, indices);
    }
    
    void Mesh::Render(ID3D12GraphicsCommandList* commandList) const
    {
        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer.GetView());
        commandList->DrawIndexedInstanced(m_indexBuffer.GetIndexCount(), 1, 0, 0, 0);
    }
}

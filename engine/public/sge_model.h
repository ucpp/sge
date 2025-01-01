#ifndef _SGE_MODEL_H_
#define _SGE_MODEL_H_

#include <vector>
#include "sge_mesh.h"
#include "sge_index_buffer.h"
#include "sge_vertex_buffer.h"

namespace SGE
{
    class Model
    {
    public:
        void Initialize(const std::vector<Mesh>& meshes, class Device* device, class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        void Render(ID3D12GraphicsCommandList* commandList) const;

    private:
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        uint32 m_indexCount = 0;
    };
}

#endif // !_SGE_MODEL_H_
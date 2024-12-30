#ifndef _SGE_MODEL_H_
#define _SGE_MODEL_H_

#include <vector>
#include "sge_mesh.h"
#include "sge_vertex_buffer.h"
#include "sge_index_buffer.h"
#include "sge_device.h"

namespace SGE
{
    class Model
    {
    public:
        void Initialize(const std::vector<Mesh>& meshes, Device* device);
        void Render(ID3D12GraphicsCommandList* commandList) const;

    private:
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        UINT m_indexCount = 0;
    };
}

#endif // !_SGE_MODEL_H_
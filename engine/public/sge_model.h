#ifndef _SGE_MODEL_H_
#define _SGE_MODEL_H_

#include <vector>
#include "sge_mesh.h"
#include "sge_index_buffer.h"
#include "sge_vertex_buffer.h"
#include "sge_constant_buffer.h"

namespace SGE
{
    class Model
    {
    public:
        void Initialize(const std::vector<Mesh>& meshes, class Device* device, class DescriptorHeap* descriptorHeap, uint32 descriptorIndex);
        void Update(const Matrix& viewMatrix, const Matrix& projectionMatrix);
        void Render(ID3D12GraphicsCommandList* commandList) const;

        void SetPosition(const Vector3& position) { m_position = position; }
        void SetRotation(const Vector3& rotation) { m_rotation = rotation; }
        void SetScale(const Vector3& scale) { m_scale = scale; }

        Vector3 GetPosition() const { return m_position; }
        Vector3 GetRotation() const { return m_rotation; }
        Vector3 GetScale() const { return m_scale; }

        Matrix GetWorldMatrix() const;

    private:
        DescriptorHeap* m_descriptorHeap = nullptr;
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        uint32 m_indexCount = 0;
        uint32 m_descriptorIndex = 0;

        Vector3 m_position = { 0.0f, 0.0f, 0.0f };
        Vector3 m_rotation = { 0.0f, 0.0f, 0.0f };
        Vector3 m_scale = { 1.0f, 1.0f, 1.0f };

        std::unique_ptr<ConstantBuffer> m_transformBuffer;
        std::vector<MeshResourceInfo> m_meshResourceInfo;
    };
}

#endif // !_SGE_MODEL_H_
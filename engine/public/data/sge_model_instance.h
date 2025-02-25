#ifndef _SGE_MODEL_INSTANCE_H_
#define _SGE_MODEL_INSTANCE_H_

#include <vector>
#include "data/sge_mesh.h"
#include "core/sge_index_buffer.h"
#include "core/sge_vertex_buffer.h"
#include "core/sge_constant_buffer.h"
#include "data/sge_model_asset.h"
#include "data/sge_material.h"

namespace SGE
{
    class ModelInstance
    {
    public:
        void Initialize(ModelAsset* asset, class Device* device, class DescriptorHeap* descriptorHeap, uint32 instanceIndex);
        void SetMaterial(Material* material);
        void UpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix);
        void Render(ID3D12GraphicsCommandList* commandList) const;
        virtual void FixedUpdate(float deltaTime);

        void SetName(const std::string& name);
        void SetActive(bool isEnabled);

        void SetPosition(const float3& position) { m_position = position; }
        void SetRotation(const float3& rotation) { m_rotation = rotation; }
        void SetScale(const float3& scale) { m_scale = scale; }

        const float3& GetPosition() const { return m_position; }
        const float3& GetRotation() const { return m_rotation; }
        const float3& GetScale() const { return m_scale; }

    protected:
        virtual const std::vector<Mesh>& GetMeshes() const;
        virtual void OnUpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix);
        float4x4 GetWorldMatrix() const;
        ConstantBuffer  m_transformBuffer;
        TransformBuffer m_transformData;

    private:
        ModelAsset*     m_asset = nullptr;
        Material*       m_material = nullptr;
        DescriptorHeap* m_descriptorHeap = nullptr;
        VertexBuffer    m_vertexBuffer;
        IndexBuffer     m_indexBuffer;
       

        float3 m_position = { 0.0f, 0.0f, 0.0f };
        float3 m_rotation = { 0.0f, 0.0f, 0.0f };
        float3 m_scale    = { 1.0f, 1.0f, 1.0f };

        uint32 m_instanceIndex = 0;
        bool m_enabled = true;
        std::string m_name = "Unnamed model";
    };
}

#endif // !_SGE_MODEL_INSTANCE_H_
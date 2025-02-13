#include "data/sge_model_instance.h"

#include "core/sge_helpers.h"
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "data/sge_texture_manager.h"
#include "core/sge_scoped_event.h"

namespace SGE
{
    void ModelInstance::Initialize(ModelAsset* asset, Device* device, DescriptorHeap* descriptorHeap, uint32 instanceIndex)
    {
        Verify(asset, "ModelInstance::Initialize asset is null.");
        
        m_asset = asset;
        m_descriptorHeap = descriptorHeap;
        m_instanceIndex = instanceIndex;
        m_vertexBuffer.Initialize(device, asset->GetVertices());
        m_indexBuffer.Initialize(device, asset->GetIndices());
        m_transformBuffer.Initialize(device->GetDevice().Get(), descriptorHeap, sizeof(TransformBuffer), m_instanceIndex);
    }

    void ModelInstance::SetMaterial(Material* material)
    {
        m_material = material;
    }

    void ModelInstance::Render(ID3D12GraphicsCommandList* commandList) const
    {
        if(!m_enabled)
        {
            return;
        }

        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer.GetView());
        commandList->SetGraphicsRootDescriptorTable(1, m_descriptorHeap->GetGPUHandle(m_instanceIndex));
        m_material->Bind(commandList, m_descriptorHeap);

        std::string eventName = "Draw " + m_name;
        SCOPED_EVENT_GPU(commandList, eventName.c_str());

        const std::vector<Mesh>& meshes = m_asset->GetMeshes();
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            const auto& resourceInfo = meshes[i].GetInfo();
            uint32 meshIndexCount = resourceInfo.meshIndexCount;
            uint32 vertexOffset = resourceInfo.vertexCountOffset;
            uint32 indexOffset = resourceInfo.indexCountOffset;

            commandList->DrawIndexedInstanced(meshIndexCount, 1, indexOffset, 0, 0);
        }
    }

    void ModelInstance::SetName(const std::string& name)
    {
        m_name = name;
    }

    void ModelInstance::SetActive(bool isEnabled)
    {
        m_enabled = isEnabled;
    }

    float4x4 ModelInstance::GetWorldMatrix() const
    {
        float4x4 scaleMatrix = CreateScaleMatrix(m_scale);
        float4x4 rotationMatrix = CreateRotationMatrixYawPitchRoll(
            ConvertToRadians(m_rotation.x),
            ConvertToRadians(m_rotation.y),
            ConvertToRadians(m_rotation.z)
        );
        float4x4 translationMatrix = CreateTranslationMatrix(m_position);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    void ModelInstance::Update(const float4x4& viewMatrix, const float4x4& projectionMatrix)
    {
        if(!m_enabled)
        {
            return;
        }

        TransformBuffer transformData = {};
        transformData.model = GetWorldMatrix();
        transformData.view = viewMatrix;
        transformData.projection = projectionMatrix;

        m_transformBuffer.Update(&transformData, sizeof(TransformBuffer));
    }
}
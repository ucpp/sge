#include "sge_model.h"

#include "sge_helpers.h"
#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_texture_manager.h"

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
        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer.GetView());
        commandList->SetGraphicsRootDescriptorTable(1, m_descriptorHeap->GetGPUHandle(m_instanceIndex));
        m_material->Bind(commandList, m_descriptorHeap);

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

    Matrix ModelInstance::GetWorldMatrix() const
    {
        Matrix scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
        Matrix rotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(m_rotation.x),
            XMConvertToRadians(m_rotation.y),
            XMConvertToRadians(m_rotation.z)
        );
        Matrix translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    void ModelInstance::Update(const Matrix& viewMatrix, const Matrix& projectionMatrix)
    {
        TransformBuffer transformData = {};
        transformData.model = GetWorldMatrix().Transpose();
        transformData.view = viewMatrix;
        transformData.projection = projectionMatrix;

        m_transformBuffer.Update(&transformData, sizeof(TransformBuffer));
    }
}
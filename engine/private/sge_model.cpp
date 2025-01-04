#include "sge_model.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_texture_manager.h"

namespace SGE
{
    void Model::Initialize(const std::vector<Mesh>& meshes, Device* device, DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        m_descriptorHeap = descriptorHeap;
        m_descriptorIndex = descriptorIndex;

        size_t totalVertexCount = 0;
        size_t totalIndexCount = 0;

        for (const auto& mesh : meshes)
        {
            totalVertexCount += mesh.GetVertices().size();
            totalIndexCount += mesh.GetIndices().size();
        }

        std::vector<Vertex> allVertices;
        std::vector<uint32> allIndices;
        allVertices.reserve(totalVertexCount);
        allIndices.reserve(totalIndexCount);

        uint32 currentIndexOffset = 0;

        for (const auto& mesh : meshes)
        {
            const auto& vertices = mesh.GetVertices();
            const auto& indices = mesh.GetIndices();

            allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());

            for (const auto& index : indices)
            {
                allIndices.push_back(index + currentIndexOffset);
            }

            MaterialData materialData = mesh.GetMaterial();
            MeshResourceInfo resourceInfo;

            resourceInfo.diffuseTextureIndex = TextureManager::GetTextureIndex(materialData.diffuseTexturePath, device, descriptorHeap);
            resourceInfo.normalTextureIndex = TextureManager::GetTextureIndex(materialData.normalTexturePath, device, descriptorHeap);
            resourceInfo.specularTextureIndex = TextureManager::GetTextureIndex(materialData.specularTexturePath, device, descriptorHeap);

            resourceInfo.vertexCountOffset = currentIndexOffset;
            resourceInfo.indexCountOffset = currentIndexOffset;
            resourceInfo.meshIndexCount = static_cast<uint32>(indices.size());

            m_meshResourceInfo.push_back(resourceInfo);

            currentIndexOffset += static_cast<uint32>(vertices.size());
        }

        std::reverse(allIndices.begin(), allIndices.end());

        m_vertexBuffer.Initialize(device, std::move(allVertices));
        m_indexBuffer.Initialize(device, std::move(allIndices));
        m_indexCount = static_cast<uint32>(allIndices.size());

        m_transformBuffer = std::make_unique<ConstantBuffer>();
        m_transformBuffer->Initialize(device->GetDevice().Get(), descriptorHeap, sizeof(TransformBuffer), descriptorIndex);
    }

    void Model::Render(ID3D12GraphicsCommandList* commandList) const
    {
        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer.GetView());

        commandList->SetGraphicsRootDescriptorTable(0, m_descriptorHeap->GetGPUHandle(m_descriptorIndex));

        for (size_t i = 0; i < m_meshResourceInfo.size(); ++i)
        {
            const auto& resourceInfo = m_meshResourceInfo[i];

            commandList->SetGraphicsRootDescriptorTable(2, m_descriptorHeap->GetGPUHandle(resourceInfo.diffuseTextureIndex));
            commandList->SetGraphicsRootDescriptorTable(3, m_descriptorHeap->GetGPUHandle(resourceInfo.normalTextureIndex));
            commandList->SetGraphicsRootDescriptorTable(4, m_descriptorHeap->GetGPUHandle(resourceInfo.specularTextureIndex));

            uint32 meshIndexCount = resourceInfo.meshIndexCount;
            uint32 vertexOffset = resourceInfo.vertexCountOffset;
            uint32 indexOffset = resourceInfo.indexCountOffset;

            commandList->DrawIndexedInstanced(meshIndexCount, 1, indexOffset, 0, 0);
        }
    }

    XMMATRIX Model::GetWorldMatrix() const
    {
        XMMATRIX scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(m_rotation.x),
            XMConvertToRadians(m_rotation.y),
            XMConvertToRadians(m_rotation.z)
        );
        XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    void Model::Update(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
    {
        TransformBuffer transformData = {};
        transformData.model = GetWorldMatrix();
        transformData.view = viewMatrix;
        transformData.projection = projectionMatrix;

        m_transformBuffer->Update(&transformData, sizeof(TransformBuffer));
    }
}
#include "sge_model.h"

#include "sge_device.h"

namespace SGE
{
    void Model::Initialize(const std::vector<Mesh>& meshes, Device* device)
    {
        size_t totalVertexCount = 0;
        size_t totalIndexCount = 0;

        for (const auto& mesh : meshes)
        {
            totalVertexCount += mesh.GetVertices().size();
            totalIndexCount += mesh.GetIndices().size();
        }

        std::vector<Vertex> allVertices;
        std::vector<UINT> allIndices;
        allVertices.reserve(totalVertexCount);
        allIndices.reserve(totalIndexCount);

        UINT currentIndexOffset = 0;

        for (const auto& mesh : meshes)
        {
            const auto& vertices = mesh.GetVertices();
            const auto& indices = mesh.GetIndices();

            allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());

            for (const auto& index : indices)
            {
                allIndices.push_back(index + currentIndexOffset);
            }

            currentIndexOffset += static_cast<UINT>(vertices.size());
        }

        m_vertexBuffer.Initialize(device, std::move(allVertices));
        m_indexBuffer.Initialize(device, std::move(allIndices));
        m_indexCount = static_cast<UINT>(allIndices.size());
    }

    void Model::Render(ID3D12GraphicsCommandList* commandList) const
    {
        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer.GetView());
        commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
    }
}
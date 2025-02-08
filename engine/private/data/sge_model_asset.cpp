#include "data/sge_model_asset.h"

#include "data/sge_mesh.h"

namespace SGE
{
    void ModelAsset::Initialize(std::vector<Mesh>& meshes)
    {
        uint32 totalVertexCount = 0;
        uint32 totalIndexCount = 0;

        for (const Mesh& mesh : meshes)
        {
            totalVertexCount += static_cast<uint32>(mesh.GetVertices().size());
            totalIndexCount += static_cast<uint32>(mesh.GetIndices().size());
        }

        m_meshes.reserve(meshes.size());
        m_vertices.reserve(totalVertexCount);
        m_indices.reserve(totalIndexCount);

        uint32 currentIndexOffset = 0;

        for (Mesh& mesh : meshes)
        {
            const std::vector<Vertex>& meshVertices = mesh.GetVertices();
            const std::vector<uint32>& meshIndices = mesh.GetIndices();

            m_vertices.insert(m_vertices.end(), meshVertices.begin(), meshVertices.end());

            for (const uint32& index : meshIndices)
            {
                m_indices.push_back(index + currentIndexOffset);
            }

            MeshResourceInfo resourceInfo{};
            resourceInfo.vertexCountOffset = currentIndexOffset;
            resourceInfo.indexCountOffset = currentIndexOffset;
            resourceInfo.meshIndexCount = static_cast<uint32>(meshIndices.size());
            mesh.UpdateInfo(resourceInfo);
            m_meshes.push_back(mesh);

            currentIndexOffset += static_cast<uint32>(meshVertices.size());
        }

        //std::reverse(m_indices.begin(), m_indices.end());
    }
}
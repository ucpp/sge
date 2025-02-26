#include "data/sge_model_asset.h"

#include "data/sge_mesh.h"
#include "core/sge_logger.h"

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

    void Skeleton::AddBone(const std::string& name, int32 index, const float4x4& offsetMatrix)
    {
        Bone bone;
        bone.name = name;
        bone.index = index;
        bone.parentIndex = -1;
        bone.offsetMatrix = offsetMatrix;
        bone.transform = float4x4::Identity;
        bone.children.clear();

        m_bones.push_back(bone);
        m_boneNameToIndex[name] = index;
    }

    int32 Skeleton::GetBoneIndex(const std::string& name) const
    {
        auto it = m_boneNameToIndex.find(name);
        if (it != m_boneNameToIndex.end())
        {
            return it->second;
        }
        return -1;
    }

    const Bone& Skeleton::GetBone(int32 index) const
    {
        if (index >= 0 && index < m_bones.size())
        {
            return m_bones[index];
        }
        throw std::out_of_range("Invalid bone index");
    }

    Bone& Skeleton::GetBone(int32 index)
    {
        if (index >= 0 && index < m_bones.size())
        {
            return m_bones[index];
        }
        throw std::out_of_range("Invalid bone index");
    }

    const float4x4& Skeleton::GetBoneOffset(int32 index) const
    {
        if (index >= 0 && index < m_bones.size())
        {
            return m_bones[index].offsetMatrix;
        }
        throw std::out_of_range("Invalid bone index");
    }

    void Skeleton::PrintBoneHierarchy() const
    {
        for (const auto& bone : m_bones)
        {
            if (bone.parentIndex == -1)
            {
                PrintBoneHierarchyRecursive(bone, 0);
                //break;
            }
        }
    }
    
    void Skeleton::PrintBoneHierarchyRecursive(const Bone& bone, int32 level) const
    {
        std::string out;
        for (int32 i = 0; i < level; ++i)
        {
            out += "    ";
        }

        out += bone.name;
        LOG_INFO(out);

        for (int32 childIndex : bone.children)
        {
            const Bone& childBone = m_bones[childIndex];
            PrintBoneHierarchyRecursive(childBone, level + 1);
        }
    }

    void AnimatedModelAsset::Initialize(std::vector<Mesh>& meshes, const Skeleton& skeleton, const std::vector<Animation>& animations)
    {
        ModelAsset::Initialize(meshes);
        m_skeleton = skeleton;
        m_animations = animations;
    }
}
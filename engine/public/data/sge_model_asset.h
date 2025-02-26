#ifndef _SGE_MODEL_ASSET_H_
#define _SGE_MODEL_ASSET_H_

#include "pch.h"
#include "data/sge_mesh.h"
#include "data/sge_animation.h"

namespace SGE
{
    class ModelAsset
    {
    public:
        void Initialize(std::vector<Mesh>& meshes);
        const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        const std::vector<uint32>& GetIndices() const { return m_indices; }
    
    private:
        std::vector<Mesh> m_meshes;
        std::vector<Vertex> m_vertices;
        std::vector<uint32> m_indices;
    };

    struct Bone
    {
        std::string name;
        int32 index;
        int32 parentIndex;
        float4x4 offsetMatrix;
        float4x4 transform;
        std::vector<int32> children;
    };

    class Skeleton
    {
    public:
        void AddBone(const std::string& name, int32 index, const float4x4& offsetMatrix);
        int32 GetBoneCount() const { return static_cast<int32>(m_bones.size()); }
        int32 GetBoneIndex(const std::string& name) const;
        const Bone& GetBone(int32 index) const;
        Bone& GetBone(int32 index);
        const float4x4& GetBoneOffset(int32 index) const;
        const std::unordered_map<std::string, int32>& GetBoneNameToIndexMap() const { return m_boneNameToIndex; }
        void PrintBoneHierarchy() const;
    
    private:
        void PrintBoneHierarchyRecursive(const Bone& bone, int32 level) const;

    private:
        std::vector<Bone> m_bones;
        std::unordered_map<std::string, int32> m_boneNameToIndex;
    };

    class AnimatedModelAsset : public ModelAsset
    {
    public:
        void Initialize(std::vector<Mesh>& meshes, const Skeleton& skeleton, const std::vector<Animation>& animations);

        const Skeleton& GetSkeleton() const { return m_skeleton; }
        const std::vector<Animation>& GetAnimations() const { return m_animations; }

    private:
        Skeleton m_skeleton;
        std::vector<Animation> m_animations;
    };
}

#endif // !_SGE_MODEL_ASSET_H_
#include "data/sge_model_loader.h"

#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "data/sge_model_asset.h"
#include <filesystem>

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<ModelAsset>> ModelLoader::m_modelAssets;
    std::unordered_map<std::string, std::unique_ptr<AnimatedModelAsset>> ModelLoader::m_animatedModelAssets;

    std::unordered_map<uint32, std::unique_ptr<ModelInstance>> ModelLoader::m_modelInstances;
    std::unordered_map<uint32, std::unique_ptr<AnimatedModelInstance>> ModelLoader::m_animatedModelInstances;

    uint32 ModelLoader::m_currentModelInstanceIndex = 0;

    bool ModelLoader::LoadModel(const ModelAssetData& assetData)
    {
        if(HasAsset(assetData.name))
        {
            return true;
        }

        Assimp::Importer importer{};
        const aiScene* scene = importer.ReadFile(assetData.path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded);
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            return false;
        }

        std::vector<Mesh> meshes;
        ProcessNode(scene->mRootNode, scene, meshes, assetData.path);

        std::unique_ptr<ModelAsset> asset = std::make_unique<ModelAsset>();
        asset->Initialize(meshes);
        m_modelAssets[assetData.name] = std::move(asset);

        return true;
    }

    bool ModelLoader::LoadAnimatedModel(const AnimatedModelAssetData& assetData)
    {
        if (HasAnimatedAsset(assetData.name))
        {
            return true;
        }

        Assimp::Importer importer{};
        const aiScene* scene = importer.ReadFile(assetData.path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded);
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            return false;
        }

        std::vector<Mesh> meshes;
        ProcessNode(scene->mRootNode, scene, meshes, assetData.path);

        Skeleton skeleton = ProcessSkeleton(scene);
        std::vector<Animation> animations = ProcessAnimations(scene);

        std::unique_ptr<AnimatedModelAsset> asset = std::make_unique<AnimatedModelAsset>();
        asset->Initialize(meshes, skeleton, animations);
        m_animatedModelAssets[assetData.name] = std::move(asset);

        return true;
    }

    ModelInstance* ModelLoader::Instantiate(const ModelAssetData& assetData, RenderContext* context)
    {
        if(HasAsset(assetData.name))
        {
            ++m_currentModelInstanceIndex;
            std::unique_ptr<ModelInstance> modelInstance = std::make_unique<ModelInstance>();
            modelInstance->Initialize(m_modelAssets[assetData.name].get(), context->GetDevice(), context->GetCbvSrvUavHeap(), m_currentModelInstanceIndex);

            m_modelInstances[m_currentModelInstanceIndex] = std::move(modelInstance);

            return m_modelInstances[m_currentModelInstanceIndex].get();
        }

        return nullptr;
    }

    AnimatedModelInstance* ModelLoader::InstantiateAnimated(const AnimatedModelAssetData& assetData, RenderContext* context)
    {
        if (HasAnimatedAsset(assetData.name))
        {
            ++m_currentModelInstanceIndex;
            std::unique_ptr<AnimatedModelInstance> modelInstance = std::make_unique<AnimatedModelInstance>();
            modelInstance->Initialize(m_animatedModelAssets[assetData.name].get(), context->GetDevice(), context->GetCbvSrvUavHeap(), m_currentModelInstanceIndex);

            m_animatedModelInstances[m_currentModelInstanceIndex] = std::move(modelInstance);

            return m_animatedModelInstances[m_currentModelInstanceIndex].get();
        }

        return nullptr;
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& modelPath)
    {
        for (uint32 i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene, modelPath));
        }

        for (uint32 i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshes, modelPath);
        }
    }

    Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelPath)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            if (mesh->HasNormals())
            {
                vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.texCoords = { 0.0f, 0.0f };
            }

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                vertex.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
            }

            vertices.push_back(vertex);
        }

        for (uint32 i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (uint32 j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(std::move(vertices), std::move(indices));
    }

    Skeleton ModelLoader::ProcessSkeleton(const aiScene* scene)
    {
        Skeleton skeleton;
        for (uint32 i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            for (uint32 j = 0; j < mesh->mNumBones; ++j)
            {
                aiBone* bone = mesh->mBones[j];
                std::string boneName = bone->mName.C_Str();
                float4x4 offsetMatrix = AssimpToFloat4x4(bone->mOffsetMatrix);
    
                skeleton.AddBone(boneName, j, offsetMatrix);
            }
        }    
        return skeleton;
    }

    std::vector<Animation> ModelLoader::ProcessAnimations(const aiScene* scene)
    {
        std::vector<Animation> animations;

        for (uint32 i = 0; i < scene->mNumAnimations; ++i)
        {
            aiAnimation* aiAnim = scene->mAnimations[i];

            Animation animation;
            animation.name = aiAnim->mName.C_Str();
            animation.duration = static_cast<float>(aiAnim->mDuration);
            animation.ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);

            for (uint32 j = 0; j < aiAnim->mNumChannels; ++j)
            {
                aiNodeAnim* aiNodeAnim = aiAnim->mChannels[j];

                BoneAnimation boneAnimation;
                boneAnimation.boneName = aiNodeAnim->mNodeName.C_Str();

                for (uint32 k = 0; k < aiNodeAnim->mNumPositionKeys; ++k)
                {
                    aiVectorKey& aiKey = aiNodeAnim->mPositionKeys[k];
                    PositionKeyframe keyframe;
                    keyframe.time = static_cast<float>(aiKey.mTime);
                    keyframe.position = float3(aiKey.mValue.x, aiKey.mValue.y, aiKey.mValue.z);
                    boneAnimation.positionKeys.push_back(keyframe);
                }

                for (uint32 k = 0; k < aiNodeAnim->mNumRotationKeys; ++k)
                {
                    aiQuatKey& aiKey = aiNodeAnim->mRotationKeys[k];
                    RotationKeyframe keyframe;
                    keyframe.time = static_cast<float>(aiKey.mTime);
                    keyframe.rotation = float4(aiKey.mValue.x, aiKey.mValue.y, aiKey.mValue.z, aiKey.mValue.w);
                    boneAnimation.rotationKeys.push_back(keyframe);
                }

                for (uint32 k = 0; k < aiNodeAnim->mNumScalingKeys; ++k)
                {
                    aiVectorKey& aiKey = aiNodeAnim->mScalingKeys[k];
                    ScaleKeyframe keyframe;
                    keyframe.time = static_cast<float>(aiKey.mTime);
                    keyframe.scale = float3(aiKey.mValue.x, aiKey.mValue.y, aiKey.mValue.z);
                    boneAnimation.scaleKeys.push_back(keyframe);
                }

                animation.boneAnimations.push_back(boneAnimation);
            }

            animations.push_back(animation);
        }

        return animations;
    }

    bool ModelLoader::HasAsset(const std::string& assetName)
    {
        auto it = m_modelAssets.find(assetName);
        return it != m_modelAssets.end() && it->second;
    }

    bool ModelLoader::HasAnimatedAsset(const std::string& assetName)
    {
        auto it = m_animatedModelAssets.find(assetName);
        return it != m_animatedModelAssets.end() && it->second;
    }

    float4x4 AssimpToFloat4x4(const aiMatrix4x4& assimpMatrix)
    {
        return float4x4(
            assimpMatrix.a1, assimpMatrix.b1, assimpMatrix.c1, assimpMatrix.d1,
            assimpMatrix.a2, assimpMatrix.b2, assimpMatrix.c2, assimpMatrix.d2,
            assimpMatrix.a3, assimpMatrix.b3, assimpMatrix.c3, assimpMatrix.d3,
            assimpMatrix.a4, assimpMatrix.b4, assimpMatrix.c4, assimpMatrix.d4
        );
    }
}

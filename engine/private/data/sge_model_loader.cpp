#include "data/sge_model_loader.h"

#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "data/sge_model_asset.h"
#include <filesystem>
#include "core/sge_logger.h"

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
        LOG_INFO("-----");
        LOG_INFO(assetData.name);
        skeleton.PrintBoneHierarchy();
        LOG_INFO("-----");
        std::vector<Animation> animations = ProcessAnimations(scene, skeleton);

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

        std::vector<std::vector<float>> vertexWeights(mesh->mNumVertices);
        std::vector<std::vector<int32>> vertexIndices(mesh->mNumVertices);

        for (uint32 i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];
            for (uint32 j = 0; j < bone->mNumWeights; j++)
            {
                aiVertexWeight weight = bone->mWeights[j];
                vertexWeights[weight.mVertexId].push_back(weight.mWeight);
                vertexIndices[weight.mVertexId].push_back(i);
            }
        }

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

            vertex.boneWeights[0] = 0.0f;
            vertex.boneWeights[1] = 0.0f;
            vertex.boneWeights[2] = 0.0f;
            vertex.boneWeights[3] = 0.0f;

            vertex.boneIndices[0] = -1;
            vertex.boneIndices[1] = -1;
            vertex.boneIndices[2] = -1;
            vertex.boneIndices[3] = -1;

            std::vector<std::pair<int32, float>> bonesAndWeights;
            for (size_t j = 0; j < vertexWeights[i].size(); j++)
            {
                bonesAndWeights.emplace_back(vertexIndices[i][j], vertexWeights[i][j]);
            }

            std::sort(bonesAndWeights.begin(), bonesAndWeights.end(), [](const std::pair<int32, float>& a, const std::pair<int32, float>& b) 
            {
                return a.second > b.second;
            });

            float weightSum = 0.0f;
            for (size_t j = 0; j < bonesAndWeights.size() && j < 4; j++)
            {
                vertex.boneIndices[j] = bonesAndWeights[j].first;
                vertex.boneWeights[j] = bonesAndWeights[j].second;
                weightSum += bonesAndWeights[j].second;
            }

            if (weightSum > 0.0f)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    vertex.boneWeights[j] /= weightSum;
                }
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

    std::string NormalizeBoneName(const std::string& name)
    {
        std::string result = name;

        size_t pos = result.find("_$Assimp");
        if (pos != std::string::npos)
        {
            result = result.substr(0, pos);
        }
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);

        return result;
    }

    void ProcessBoneHierarchy(aiNode* node, const std::unordered_map<std::string, int32>& boneNameToIndex, Skeleton& skeleton, int32 parentIndex)
    {
        std::string rawBoneName = node->mName.C_Str();
        std::string boneName = NormalizeBoneName(rawBoneName);
        auto it = boneNameToIndex.find(boneName);

        int32 boneIndex = -1;
        if (it != boneNameToIndex.end())
        {
            boneIndex = it->second;
            if (skeleton.GetBone(boneIndex).parentIndex != -1)
            {
                return;
            }

            skeleton.GetBone(boneIndex).parentIndex = parentIndex;

            if (parentIndex != -1)
            {
                skeleton.GetBone(parentIndex).children.push_back(boneIndex);
            }
        }

        for (uint32 i = 0; i < node->mNumChildren; ++i)
        {
            int32 nextParentIndex = (boneIndex != -1) ? boneIndex : parentIndex;
            ProcessBoneHierarchy(node->mChildren[i], boneNameToIndex, skeleton, nextParentIndex);
        }
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
                std::string rawBoneName = bone->mName.C_Str();
                std::string boneName = NormalizeBoneName(rawBoneName);
                float4x4 offsetMatrix = AssimpToFloat4x4(bone->mOffsetMatrix);
                //LOG_INFO("Add bone: {}", boneName);
                skeleton.AddBone(boneName, j, offsetMatrix);
            }
        }

        const std::unordered_map<std::string, int32>& boneNameToIndex = skeleton.GetBoneNameToIndexMap();

        aiNode* actualRootNode = scene->mRootNode;
        while (actualRootNode->mNumChildren > 0)
        {
            bool hasBoneChildren = false;
            for (uint32 i = 0; i < actualRootNode->mNumChildren; ++i)
            {
                std::string childName = actualRootNode->mChildren[i]->mName.C_Str();
                if (boneNameToIndex.find(NormalizeBoneName(childName)) != boneNameToIndex.end())
                {
                    hasBoneChildren = true;
                    break;
                }
            }
            if (hasBoneChildren)
            {
                break;
            }
            actualRootNode = actualRootNode->mChildren[0];
        }

        ProcessBoneHierarchy(actualRootNode, boneNameToIndex, skeleton, -1);

        return skeleton;
    }

    std::vector<Animation> ModelLoader::ProcessAnimations(const aiScene* scene, const Skeleton& skeleton)
    {
        std::vector<Animation> animations;

        if (!scene->HasAnimations())
        {
            return animations;
        }

        aiNode* rootNode = scene->mRootNode;

        for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
        {
            aiAnimation* aiAnim = scene->mAnimations[i];
            Animation animation;

            animation.name = aiAnim->mName.C_Str();
            animation.duration = static_cast<float>(aiAnim->mDuration);
            animation.ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);
            
            for (unsigned int j = 0; j < aiAnim->mNumChannels; ++j)
            {
                aiNodeAnim* nodeAnim = aiAnim->mChannels[j];
                std::string rawChannelName = nodeAnim->mNodeName.C_Str();
                std::string boneName = NormalizeBoneName(rawChannelName);

                //LOG_INFO("Read Bone: {}", boneName);

                BoneKeyframes& boneKeyframes = animation.boneKeyframes[boneName];

                for (unsigned int k = 0; k < nodeAnim->mNumPositionKeys; ++k)
                {
                    const aiVectorKey& key = nodeAnim->mPositionKeys[k];
                    PositionKeyframe positionKeyframe;
                    positionKeyframe.time = static_cast<float>(key.mTime);
                    positionKeyframe.position = float3(key.mValue.x, key.mValue.y, key.mValue.z);
                    boneKeyframes.positionKeys.push_back(positionKeyframe);
                }

                for (unsigned int k = 0; k < nodeAnim->mNumRotationKeys; ++k)
                {
                    const aiQuatKey& key = nodeAnim->mRotationKeys[k];
                    RotationKeyframe rotationKeyframe;
                    rotationKeyframe.time = static_cast<float>(key.mTime);
                    rotationKeyframe.rotation = float4(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
                    boneKeyframes.rotationKeys.push_back(rotationKeyframe);
                }

                for (unsigned int k = 0; k < nodeAnim->mNumScalingKeys; ++k)
                {
                    const aiVectorKey& key = nodeAnim->mScalingKeys[k];
                    ScaleKeyframe scaleKeyframe;
                    scaleKeyframe.time = static_cast<float>(key.mTime);
                    scaleKeyframe.scale = float3(key.mValue.x, key.mValue.y, key.mValue.z);
                    boneKeyframes.scaleKeys.push_back(scaleKeyframe);
                }
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
            assimpMatrix.a1, assimpMatrix.a2, assimpMatrix.a3, assimpMatrix.a4,
            assimpMatrix.b1, assimpMatrix.b2, assimpMatrix.b3, assimpMatrix.b4,
            assimpMatrix.c1, assimpMatrix.c2, assimpMatrix.c3, assimpMatrix.c4,
            assimpMatrix.d1, assimpMatrix.d2, assimpMatrix.d3, assimpMatrix.d4
        );
    }
}

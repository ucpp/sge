#include "data/sge_model_loader.h"

#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "data/sge_model_asset.h"
#include <filesystem>

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<ModelAsset>> ModelLoader::m_modelAssets;
    std::unordered_map<uint32, std::unique_ptr<ModelInstance>> ModelLoader::m_modelInstances;
    uint32 ModelLoader::m_currentModelInstanceIndex = 0;

    bool ModelLoader::LoadModel(const ModelAssetData& assetData)
    {
        if(HasAsset(assetData.name))
        {
            return true;
        }

        Assimp::Importer importer{};
        const aiScene* scene = importer.ReadFile(assetData.path, 
          aiProcess_Triangulate | 
          aiProcess_CalcTangentSpace /* |
          aiProcess_MakeLeftHanded*/);

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
                vertex.bitangent = { -mesh->mBitangents[i].x, -mesh->mBitangents[i].y, -mesh->mBitangents[i].z };
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

    bool ModelLoader::HasAsset(const std::string& assetName)
    {
        auto it = m_modelAssets.find(assetName);
        return it != m_modelAssets.end() && it->second;
    }
}

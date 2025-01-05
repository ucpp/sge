#include "sge_model_loader.h"

#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include <filesystem>

namespace SGE
{
    Model ModelLoader::LoadModel(const std::string& filePath, Device* device, DescriptorHeap* descriptorHeap, uint32 descriptorIndex)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        }

        std::vector<Mesh> meshes;
        ProcessNode(scene->mRootNode, scene, meshes, filePath);

        Model model;
        model.Initialize(meshes, device, descriptorHeap, descriptorIndex);

        return model;
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

        MaterialData material = {};
        if (scene->mMaterials)
        {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            aiString path;

            if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                material.diffuseTexturePath = BuildTexturePath(modelPath, path.C_Str());
            }
            if (aiMat->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
            {
                material.normalTexturePath = BuildTexturePath(modelPath, path.C_Str());
            }
            if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
            {
                material.specularTexturePath = BuildTexturePath(modelPath, path.C_Str());
            }
        }

        return Mesh(std::move(vertices), std::move(indices), material);
    }

    std::string ModelLoader::BuildTexturePath(const std::string& modelPath, const std::string& textureRelativePath)
    {
        std::filesystem::path modelDir = std::filesystem::path(modelPath).parent_path();
        std::filesystem::path texturePath = modelDir / textureRelativePath;

        return texturePath.string();
    }
}

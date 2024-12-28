#include "sge_model_loader.h"

#include <stdexcept>

namespace SGE
{
    Model ModelLoader::LoadModel(const std::string& filePath, Device* device)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);
        
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        }

        Model model;
        ProcessNode(scene->mRootNode, scene, model, device);
        return model;
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Model& model, Device* device)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.AddMesh(ProcessMesh(mesh, scene, device));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model, device);
        }
    }

    Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, Device* device)
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
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

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        Mesh resultMesh;
        resultMesh.Initialize(vertices, indices, device);

        return resultMesh;
    }
}

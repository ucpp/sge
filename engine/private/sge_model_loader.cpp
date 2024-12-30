#include "sge_model_loader.h"

#include <stdexcept>

namespace SGE
{
    Model ModelLoader::LoadModel(const std::string& filePath, Device* device)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        }

        std::vector<Mesh> meshes;
        ProcessNode(scene->mRootNode, scene, meshes);

        Model model;
        model.Initialize(meshes, device);

        return model;
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshes);
        }
    }

    Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

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

        return Mesh(std::move(vertices), std::move(indices));
    }
}
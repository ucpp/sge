#include "model.h"

#include <iostream>
#include "mesh.h"
#include "resource_manager.h"

namespace Engine
{
    void Model::Load(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "Assimp error load scene: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory_ = path.substr(0, path.find_last_of('/'));

        LoadNode(scene->mRootNode, scene);
    }

    void Model::LoadNode(aiNode* node, const aiScene* scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }

        for(unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            LoadNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            glm::vec3 v3;

            v3.x = mesh->mVertices[i].x;
            v3.y = mesh->mVertices[i].y;
            v3.z = mesh->mVertices[i].z;
            vertex.position = v3;

            v3.x = mesh->mNormals[i].x;
            v3.y = mesh->mNormals[i].y;
            v3.z = mesh->mNormals[i].z;
            vertex.normal = v3;

            if(mesh->mTextureCoords[0])
            {
                glm::vec2 v2;

                v2.x = mesh->mTextureCoords[0][i].x;
                v2.y = mesh->mTextureCoords[0][i].y;
                vertex.uv = v2;
            }
            else
            {
                vertex.uv = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];

            for(unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        textures.push_back(LoadTexture(material, aiTextureType_DIFFUSE, "diffuse"));
        //textures.push_back(LoadTexture(material, aiTextureType_NORMALS, "normal"));
        //textures.push_back(LoadTexture(material, aiTextureType_SPECULAR, "specular"));

        Mesh result_mesh;
        result_mesh.Init(vertices, indices, textures);

        return result_mesh;
    }

    Texture Model::LoadTexture(aiMaterial* material, aiTextureType type, std::string type_name)
    {
        aiString path;
        material->GetTexture(type, 0, &path);
        std::string filename = std::string(path.C_Str());
        filename = this->directory_ + "/" + filename;
        Texture texture = Engine::ResourceManager::LoadTexture(filename.c_str(), path.C_Str(), type_name);

        return texture;
    }

    void Model::Draw(Shader& shader)
    {
        for(auto mesh : meshes)
        {
            mesh.Draw(shader);
        }
    }

    void Model::Clear()
    {
        for(auto mesh : meshes)
        {
            mesh.Clear();
        }

        meshes.clear();
    }
}
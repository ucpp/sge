#include "model.h"

#include <iostream>
#include "mesh.h"
#include "resource_manager.h"

namespace sge
{
	void Model::load(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Assimp error load scene: " << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		count_vertices = 0;

		loadNode(scene->mRootNode, scene);
		importer.FreeScene();
	}

	void Model::loadNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			loadNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
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

			if (mesh->mTextureCoords[0])
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

			glm::vec3 tg;
			tg.x = mesh->mTangents[i].x;
			tg.x = mesh->mTangents[i].x;
			tg.x = mesh->mTangents[i].x;

			vertex.tangent = tg;

			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		textures.push_back(loadTexture(material, aiTextureType_DIFFUSE, "material.diffuse"));
		textures.push_back(loadTexture(material, aiTextureType_HEIGHT, "material.normal"));
		textures.push_back(loadTexture(material, aiTextureType_SPECULAR, "material.specular"));
		// textures.push_back(LoadTexture(material, aiTextureType_AMBIENT, "material.ambient"));

		Mesh result_mesh;
		result_mesh.initialize(vertices, indices, textures);
		count_vertices += result_mesh.getCountVertices();

		return result_mesh;
	}

	Texture Model::loadTexture(aiMaterial* material, aiTextureType type, std::string type_name)
	{
		Texture texture;

		if (material->GetTextureCount(type) > 0)
		{
			aiString path;
			if (material->GetTexture(type, 0, &path) == AI_SUCCESS)
			{
				std::string filename = std::string(path.C_Str());
				filename = this->directory + "/" + filename;
				// std::cout << filename.c_str() << std::endl;

				std::string tex_name(path.C_Str());
				tex_name = tex_name.substr(0, tex_name.find_last_of('.'));
				tex_name += "_" + type_name;
				texture = sge::ResourceManager::loadTexture(filename, tex_name, type_name);
			}
		}
		else
		{
			// std::cout << "error loading texture with type " << type  << std::endl;
		}

		return texture;
	}

	void Model::draw(Shader& shader, int cubemap_texture)
	{
		for (auto mesh : meshes)
		{
			mesh.draw(shader, cubemap_texture);
		}
	}

	void Model::clear()
	{
		for (auto mesh : meshes)
		{
			mesh.clear();
		}

		meshes.clear();
	}

	uint32_t Model::getCountVertices() const
	{
		return count_vertices;
	}
}
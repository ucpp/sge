#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include "mesh.h"

namespace sge
{
    class Model
    {
    public:
        void Load(const std::string &path);
        void Draw(Shader &shader);
        void Clear();

    public:
        std::vector<Mesh> meshes;

    private:
        void LoadNode(aiNode *node, const aiScene *scene);
        Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
        Texture LoadTexture(aiMaterial *material, aiTextureType type, std::string type_name, std::string force_path = "");

    private:
        std::string directory_;
    };
}
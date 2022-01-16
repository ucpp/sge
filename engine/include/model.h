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
        void load(const std::string &path);
        void draw(Shader &shader);
        void clear();

    public:
        std::vector<Mesh> meshes;

    private:
        void loadNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        Texture loadTexture(aiMaterial *material, aiTextureType type, std::string type_name, std::string force_path = "");

    private:
        std::string directory;
    };
}
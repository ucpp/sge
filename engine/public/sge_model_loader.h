#ifndef _SGE_MODEL_LOADER_H_
#define _SGE_MODEL_LOADER_H_

#include <string>
#include "sge_model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SGE
{
    class ModelLoader
    {
    public:
        static Model LoadModel(const std::string& filePath, class Device* device);

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& modelPath);
        static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelPath);
        static std::string BuildTexturePath(const std::string& modelPath, const std::string& textureRelativePath);
    };
}

#endif // !_SGE_MODEL_LOADER_H_
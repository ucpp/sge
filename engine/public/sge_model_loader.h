#ifndef _SGE_MODEL_LOADER_H_
#define _SGE_MODEL_LOADER_H_

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "sge_device.h"
#include "sge_model.h"
#include "sge_mesh.h"

namespace SGE
{
    class ModelLoader
    {
    public:
        static Model LoadModel(const std::string& filePath, Device* device);

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, Model& model, Device* device);
        static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Device* device);
    };
}

#endif // !_SGE_MODEL_LOADER_H_
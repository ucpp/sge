#ifndef _SGE_MODEL_LOADER_H_
#define _SGE_MODEL_LOADER_H_

#include <string>
#include "sge_render_context.h"
#include "sge_model.h"
#include "sge_model_asset.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SGE
{
    class ModelLoader
    {
    public:
        static bool LoadModel(const ModelAssetSettings& assetSettings);
        static ModelInstance* Instantiate(const ModelAssetSettings& assetSettings, RenderContext* context);

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& modelPath);
        static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelPath);
        static bool HasAsset(const std::string& assetName);

    private:
        static std::unordered_map<std::string, std::unique_ptr<ModelAsset>> m_modelAssets;
        static std::unordered_map<uint32, std::unique_ptr<ModelInstance>> m_modelInstances;
        static uint32 m_currentModelInstanceIndex;
    };
}

#endif // !_SGE_MODEL_LOADER_H_
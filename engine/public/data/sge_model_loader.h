#ifndef _SGE_MODEL_LOADER_H_
#define _SGE_MODEL_LOADER_H_

#include <string>
#include "rendering/sge_render_context.h"
#include "data/sge_model_instance.h"
#include "data/sge_model_asset.h"
#include "data/sge_animated_model_instance.h"
#include "data/sge_data_structures.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

namespace SGE
{
    class ModelLoader
    {
    public:
        static bool LoadModel(const ModelAssetData& assetData);
        static bool LoadAnimatedModel(const AnimatedModelAssetData& assetData);

        static ModelInstance* Instantiate(const ModelAssetData& assetSettings, RenderContext* context);
        static AnimatedModelInstance* InstantiateAnimated(const AnimatedModelAssetData& assetSettings, RenderContext* context);

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& modelPath);
        static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelPath);

        static Skeleton ProcessSkeleton(const aiScene* scene);
        static std::vector<Animation> ProcessAnimations(const aiScene* scene, const Skeleton& skeleton);

        static bool HasAsset(const std::string& assetName);
        static bool HasAnimatedAsset(const std::string& assetName);

    private:
        static std::unordered_map<std::string, std::unique_ptr<ModelAsset>> m_modelAssets;
        static std::unordered_map<std::string, std::unique_ptr<AnimatedModelAsset>> m_animatedModelAssets;

        static std::unordered_map<uint32, std::unique_ptr<ModelInstance>> m_modelInstances;
        static std::unordered_map<uint32, std::unique_ptr<AnimatedModelInstance>> m_animatedModelInstances;
        static uint32 m_currentModelInstanceIndex;
    };

    float4x4 AssimpToFloat4x4(const aiMatrix4x4& assimpMatrix);
}

#endif // !_SGE_MODEL_LOADER_H_
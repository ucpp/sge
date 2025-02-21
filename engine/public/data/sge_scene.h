#ifndef _SGE_SCENE_H_
#define _SGE_SCENE_H_

#include "pch.h"
#include "data/sge_camera.h"
#include "data/sge_camera_controller.h"
#include "data/sge_data_structures.h"
#include "core/sge_constant_buffer.h"
#include "data/sge_model_instance.h"
#include "data/sge_animated_model_instance.h"

namespace SGE
{
    class Scene
    {
    public:
        void Initialize(class RenderContext* context);
        void Update(double deltaTime);
        void Shutdown();

        const std::map<const ModelData*, ModelInstance*>& GetModels() const { return m_modelInstances; }
        const std::map<const AnimatedModelData*, AnimatedModelInstance*>& GetAnimModels() const { return m_animatedModelInstances; }

        CubemapAssetData GetSkyboxCubeMap() const { return m_skyboxCubemap; }

    private:
        void InitializeCamera();
        void InitializeFrameData();
        void InstantiateModels();
        void InstantiateAnimatedModels();

        void UpdateCamera(double deltaTime);
        void UpdateModels(double deltaTime);
        void SyncFrameData();

    private:
        class RenderContext* m_context = nullptr;
        
        Camera m_mainCamera;
        CameraController m_cameraController;
        std::map<const ModelData*, ModelInstance*> m_modelInstances;
        std::map<const AnimatedModelData*, AnimatedModelInstance*> m_animatedModelInstances;

        CubemapAssetData m_skyboxCubemap;

        std::unique_ptr<ConstantBuffer> m_frameDataBuffer;
        FrameData m_frameData;
    };
}

#endif // !_SGE_SCENE_H_
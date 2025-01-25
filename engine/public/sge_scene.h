#ifndef _SGE_SCENE_H_
#define _SGE_SCENE_H_

#include "pch.h"
#include "sge_camera.h"
#include "sge_camera_controller.h"
#include "sge_application_settings.h"
#include "sge_scene_data.h"
#include "sge_constant_buffer.h"
#include "sge_model.h"

namespace SGE
{
    class Scene
    {
    public:
        void Initialize(class RenderContext* context);
        void Update(double deltaTime);
        void Shutdown();

        const std::map<const ModelData*, ModelInstance*>& GetModels() const { return m_modelInstances; }

    private:
        void InitializeCamera();
        void InitializeSceneData();
        void InstantiateModels();
        void InitializeFog();

        void UpdateCamera(double deltaTime);
        void UpdateSceneData(double deltaTime);
        void UpdateModels(double deltaTime);

    private:
        class RenderContext* m_context = nullptr;
        
        Camera m_mainCamera;
        CameraController m_cameraController;
        std::map<const ModelData*, ModelInstance*> m_modelInstances;

        std::unique_ptr<ConstantBuffer> m_sceneDataBuffer;
        SceneData m_sceneData;
    };
}

#endif // !_SGE_SCENE_H_
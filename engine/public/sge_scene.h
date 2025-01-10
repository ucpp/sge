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

        const std::vector<std::unique_ptr<Model>>& GetRenderableObjects() const { return m_renderableObjects; }

    private:
        void InitializeCamera();
        void InitializeSceneData();
        void InitializeRenderableObjects();

        void UpdateCamera(double deltaTime);
        void UpdateSceneData(double deltaTime);
        void UpdateRenderableObjects(double deltaTime);

    private:
        class RenderContext* m_context = nullptr;
        
        Camera m_mainCamera;
        CameraController m_cameraController;
        std::vector<std::unique_ptr<Model>> m_renderableObjects;

        std::unique_ptr<ConstantBuffer> m_sceneDataBuffer;
        SceneData m_sceneData;
    };
}

#endif // !_SGE_SCENE_H_
#ifndef _SGE_RENDERER_H_
#define _SGE_RENDERER_H_

#include "pch.h"

#include "sge_camera.h"
#include "sge_camera_controller.h"
#include "sge_constant_buffer.h"
#include "sge_device.h"
#include "sge_depth_buffer.h"
#include "sge_descriptor_heap.h"
#include "sge_editor.h"
#include "sge_fence.h"
#include "sge_model.h"
#include "sge_pipeline_state.h"
#include "sge_render_target.h"
#include "sge_root_signature.h"
#include "sge_shader.h"
#include "sge_viewport_scissors.h"
#include "sge_non_copyable.h"
#include "sge_scene_data.h"
#include "sge_scene.h"

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(class RenderContext* context);
        void Update(double deltaTime);
        void Render(Scene* scene, Editor* editor);
        void Shutdown();

    private:
        void InitializeCamera();
        void InitializePipelineStates();
        void InitializeSceneBuffers();

        void UpdateSceneDataBuffer();
        void UpdateModelBuffer();

        void BeginFrame();
        void RenderGeometryPass();
        void EndFrame();

        PipelineState* GetActivePipelineState() const;

    private:
        class RenderContext* m_context = nullptr;

        std::unique_ptr<PipelineState> m_forwardPipelineState;
        std::unique_ptr<PipelineState> m_deferredPipelineState;

        Fence m_fence;

        Camera m_camera;
        CameraController m_cameraController;

        std::unique_ptr<Model> m_model;

        std::unique_ptr<ConstantBuffer> m_sceneDataBuffer;

        bool m_playAnimation = false;
        SceneData m_sceneData;
    };
}

#endif // !_SGE_RENDERER_H_

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
#include "sge_gbuffer.h"

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(class RenderContext* context);
        void Render(Scene* scene, Editor* editor);
        void Shutdown();

    private:
        void InitializePipelineStates();
        void ForwardRendering(Scene* scene);
        void DeferredRendering(Scene* scene);
        void LightingPass();

        PipelineState* GetActivePipelineState() const;

    private:
        class RenderContext* m_context = nullptr;

        std::unique_ptr<PipelineState> m_forwardPipelineState;
        std::unique_ptr<PipelineState> m_deferredPipelineState;
        std::unique_ptr<PipelineState> m_lightingPipelineState;
    };
}

#endif // !_SGE_RENDERER_H_

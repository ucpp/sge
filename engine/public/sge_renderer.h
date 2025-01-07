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

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(class Window* window, struct ApplicationSettings* settings);
        void Update(double deltaTime);
        void Render();
        void Shutdown();

        void ResizeScreen(uint32 width, uint32 height);

    private:
        void InitializeCamera();
        void InitializeDescriptorHeaps();
        void InitializeShaders();
        void InitializePipelineStates();
        void InitializeRenderTargets();
        void InitializeSceneBuffers();

        void UpdateBuffers();
        void UpdateSceneDataBuffer();
        void UpdateModelBuffer();

        void BeginFrame();
        void ClearRenderTargets();
        void RenderGeometryPass();
        void RenderImGuiPass();
        void WaitForPreviousFrame();
        void ExecuteCommandList();
        void EndFrame();

        ID3D12PipelineState* GetActivePipelineState() const;

    private:
        class Window* m_window = nullptr;
        struct ApplicationSettings* m_settings = nullptr;

        std::unique_ptr<Device> m_device;
        std::unique_ptr<ViewportScissors> m_viewportScissors;
        std::unique_ptr<RenderTarget> m_renderTarget;
        std::unique_ptr<DepthBuffer> m_depthBuffer;

        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        std::unique_ptr<RootSignature> m_rootSignature;
        std::unique_ptr<PipelineState> m_pipelineState;
        std::unique_ptr<PipelineState> m_wireframePipelineState;

        Fence m_fence;
        UINT m_frameIndex;

        Camera m_camera;
        CameraController m_cameraController;

        std::unique_ptr<Model> m_model;
        std::unique_ptr<Editor> m_editor;

        DescriptorHeap m_cbvSrvUavHeap;
        DescriptorHeap m_rtvHeap;
        DescriptorHeap m_dsvHeap;

        std::unique_ptr<ConstantBuffer> m_sceneDataBuffer;

        bool m_playAnimation = false;
        SceneData m_sceneData;
    };
}

#endif // !_SGE_RENDERER_H_

#ifndef _SGE_RENDERER_H_
#define _SGE_RENDERER_H_

#include <memory>
#include <vector>
#include "sge_non_copyable.h"
#include "sge_device.h"
#include "sge_window.h"
#include "sge_viewport_scissors.h"
#include "sge_shader.h"
#include "sge_root_signature.h"
#include "sge_pipeline_state.h"
#include "sge_vertex_buffer.h"
#include "sge_index_buffer.h"
#include "sge_descriptor_heap.h"
#include "sge_constant_buffer.h"
#include "sge_fence.h"
#include "sge_render_target.h"

#include "sge_camera.h"
#include "sge_camera_controller.h"
#include "sge_model.h"

#include <wrl.h>
using namespace Microsoft::WRL;

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(Window* window);
        void Update(double deltaTime);
        void Render();
        void Shutdown();

    private:
        void InitializeCamera();
        void PopulateCommandList();
        void WaitForPreviousFrame();
        void BuildImGuiFrame();
    
    private:
        std::unique_ptr<Device> m_device;
        Window* m_window = nullptr;
        
        std::unique_ptr<ViewportScissors> m_viewportScissors;
        std::unique_ptr<RenderTarget> m_renderTarget;
        
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        std::unique_ptr<RootSignature> m_rootSignature;
        std::unique_ptr<PipelineState> m_pipelineState;

        std::unique_ptr<DescriptorHeap> m_descriptorHeap;

        Fence m_fence;
        UINT m_frameIndex;

        Camera m_camera;
        CameraController m_cameraController;

        std::unique_ptr<ConstantBuffer> m_modelBuffer;
        std::unique_ptr<ConstantBuffer> m_viewBuffer;
        std::unique_ptr<ConstantBuffer> m_projectionBuffer;

        std::unique_ptr<Model> m_model;
    };
}

#endif // !_SGE_RENDERER_H_
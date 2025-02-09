#ifndef _SGE_RENDER_PASS_H_
#define _SGE_RENDER_PASS_H_

#include "pch.h"
#include "rendering/sge_pipeline_state.h"
#include "rendering/sge_render_target_texture.h"

namespace SGE
{
    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        void Initialize(class RenderContext* context);
        void Render(class Scene* scene);
        void Shutdown();

        void Reload() { m_reloadRequested = true; }

    protected:
        virtual void OnInitialize(class RenderContext* context) {};
        virtual void OnRender(class Scene* scene) {};
        virtual void OnShutdown() {};
        
        virtual PipelineConfig GetPipelineConfig() const = 0;
        static PipelineConfig CreateFullscreenQuadPipelineConfig(DXGI_FORMAT renderTargetFormat, const std::string& pixelShaderPath);

        void SetTargetState(RTargetType type, D3D12_RESOURCE_STATES state);
        void ClearRenderTargetView(RTargetType type);
        void ClearRenderTargetView(const std::vector<RTargetType>& type);
        void SetRenderTarget(RTargetType type);
        void SetRenderTarget(const std::vector<RTargetType>& types);
        void BindRenderTargetSRV(RTargetType type, uint32 descIndex);

    protected:
        class RenderContext* m_context = nullptr;
        std::unique_ptr<PipelineState> m_pipelineState;

    private:
        bool m_reloadRequested = false;
    };
}

#endif // !_SGE_RENDER_PASS_H_
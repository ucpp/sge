#ifndef _SGE_RENDER_PASS_H_
#define _SGE_RENDER_PASS_H_

#include "pch.h"
#include "rendering/sge_pipeline_state.h"
#include "rendering/sge_render_target_texture.h"
#include "core/sge_scoped_event.h"
#include "data/sge_data_structures.h"

namespace SGE
{
    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        void Initialize(class RenderContext* context, const RenderPassData& passData, const std::string& passName = "RednderPass");
        void Render(class Scene* scene);
        void Shutdown();

        void Reload() { m_reloadRequested = true; }

    protected:
        virtual void OnInitialize(class RenderContext* context) {}
        virtual void OnRender(class Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output) {}
        virtual void OnDraw(class Scene* scene);
        virtual void OnShutdown() {}
        
        virtual PipelineConfig GetPipelineConfig() const = 0;
        static PipelineConfig CreateFullscreenQuadPipelineConfig(DXGI_FORMAT renderTargetFormat, const RenderPassData& passData);

        void SetTargetState(const std::vector<std::string>& names, D3D12_RESOURCE_STATES state);
        void SetTargetState(const std::string& name, D3D12_RESOURCE_STATES state);
        void ClearRenderTargetView(const std::string& name);
        void ClearRenderTargetView(const std::vector<std::string>& names);
        void SetRenderTarget(const std::string& name);
        void SetRenderTarget(const std::vector<std::string>& names);
        void BindRenderTargetSRV(const std::string& name, uint32 descIndex);

        void DrawQuad();
        void DrawModels(class Scene* scene);

    protected:
        class RenderContext* m_context = nullptr;
        std::unique_ptr<PipelineState> m_pipelineState;
        RenderPassData m_passData;

    private:
        bool m_reloadRequested = false;
        std::string m_name;
    };
}

#endif // !_SGE_RENDER_PASS_H_
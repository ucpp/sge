#include "rendering/passes/sge_render_pass.h"

#include "rendering/sge_render_context.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void RenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "RenderPass::Initialize: Provided render context is null.");
        m_context = context;

        OnInitialize(context);

        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), GetPipelineConfig(), m_reloadRequested);
        }
    }

    void RenderPass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        OnRender(scene);
    }

    void RenderPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
        OnShutdown();
    }

    PipelineConfig RenderPass::CreateFullscreenQuadPipelineConfig(DXGI_FORMAT renderTargetFormat, const std::string& pixelShaderPath)
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(renderTargetFormat)
            .SetDepthStencilFormat(DXGI_FORMAT_UNKNOWN, false)
            .SetSampleCount(1)
            .SetShaders("/vs_fullscreen_quad.hlsl", pixelShaderPath);
    }
}

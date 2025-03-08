#include "rendering/passes/sge_ssr_render_pass.h"
#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "data/sge_texture_manager.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void SSRRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        //commandList->SetPipelineState(m_pipelineState->GetPipelineState());
        //m_context->SetRootSignature(m_pipelineState->GetSignature());
    }

    void SSRRenderPass::OnDraw(Scene* scene)
    {
    }

    PipelineConfig SSRRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
        .SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
        .SetDepthStencilFormat(DXGI_FORMAT_UNKNOWN, false)
        .SetSampleCount(1)
        .SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
    }
}
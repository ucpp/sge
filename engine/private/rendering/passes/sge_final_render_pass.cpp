#include "rendering/passes/sge_final_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void FinalRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        const std::string finalTarget = m_context->GetRenderData().finalRender;
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget(false);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(finalTarget, 2);
    }

    PipelineConfig FinalRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "/ps_final_pass.hlsl");
    }
}
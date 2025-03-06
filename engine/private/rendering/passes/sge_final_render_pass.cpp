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
        SetTargetState(finalTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_context->GetDepthBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget(false);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(finalTarget, 2);
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetDepthBuffer()->GetSRVGPUHandle());
    }

    PipelineConfig FinalRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, m_passData);
    }
}
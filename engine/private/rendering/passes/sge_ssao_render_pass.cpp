#include "rendering/passes/sge_ssao_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void SSAORenderPass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::NormalRoughness, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_context->GetDepthBuffer()->GetResource(0)->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        SetTargetState(RTargetType::SSAOBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(RTargetType::SSAOBuffer);
        SetRenderTarget(RTargetType::SSAOBuffer);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(RTargetType::NormalRoughness, 2);
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetDepthBuffer()->GetSRVGPUHandle(0));
    }

    PipelineConfig SSAORenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "/ps_ssao_pass.hlsl");
    }
}
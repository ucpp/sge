#include "rendering/passes/sge_tonemapping_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void ToneMappingRenderPass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        m_context->GetBloomBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget(false);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetBloomBuffer()->GetSRVGPUHandle());

        commandList->DrawInstanced(6, 1, 0, 0);
    }

    PipelineConfig ToneMappingRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "shaders/ps_tonemapping_pass.hlsl");
    }
}
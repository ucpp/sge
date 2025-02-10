#include "rendering/passes/sge_tonemapping_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void ToneMappingRenderPass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::BloomBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        SetTargetState(RTargetType::ToneMapping, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(RTargetType::ToneMapping);
        SetRenderTarget(RTargetType::ToneMapping);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(RTargetType::BloomBuffer, 2);
    }

    PipelineConfig ToneMappingRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "/ps_tonemapping_pass.hlsl");
    }
}
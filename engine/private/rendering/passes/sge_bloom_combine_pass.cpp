#include "rendering/passes/sge_bloom_combine_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void BloomCombinePass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::BlurBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        SetTargetState(RTargetType::LightingBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
      
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        SetTargetState(RTargetType::BloomBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

        ClearRenderTargetView(RTargetType::BloomBuffer);
        SetRenderTarget(RTargetType::BloomBuffer);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(RTargetType::LightingBuffer, 2);
        BindRenderTargetSRV(RTargetType::BlurBuffer, 3);

        commandList->DrawInstanced(6, 1, 0, 0);
    }

    PipelineConfig BloomCombinePass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "/ps_bloom_combine_pass.hlsl");
    }
}
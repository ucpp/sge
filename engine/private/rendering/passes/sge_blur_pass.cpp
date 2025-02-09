#include "rendering/passes/sge_blur_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void BlurPass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::BrightnessBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        SetTargetState(RTargetType::BlurBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(RTargetType::BlurBuffer);
        SetRenderTarget(RTargetType::BlurBuffer);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(RTargetType::BrightnessBuffer, 2);
    }

    PipelineConfig BlurPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "/ps_blur_pass.hlsl");
    }
}
#include "rendering/passes/sge_shadow_map_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "data/sge_texture_manager.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void ShadowMapRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        m_context->GetShadowMap()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList.Get());
        CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapDSV = m_context->GetShadowMap()->GetDSVHandle();
        commandList->ClearDepthStencilView(shadowMapDSV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(0, nullptr, false, &shadowMapDSV);
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRootDescriptorTable(0, 0);
    }

    void ShadowMapRenderPass::OnDraw(Scene* scene)
    {
        DrawModels(scene);
    }
    
    PipelineConfig ShadowMapRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
            .SetSampleCount(1)
            .SetShaders("/vs_shadow_map_pass.hlsl", "/ps_empty.hlsl");
    }
}
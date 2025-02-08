#include "rendering/passes/sge_forward_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void ForwardRenderPass::OnRender(Scene* scene)
    {
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        m_context->SetRenderTarget();
        m_context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->SetRootDescriptorTable(0, 0);

        for(auto& pair : scene->GetModels())
        {
            pair.second->Render(m_context->GetCommandList().Get());
        }
    }
    
    PipelineConfig ForwardRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
            .SetSampleCount(1)
            .SetShaders("shaders/vs_forward_pass.hlsl", "shaders/ps_forward_pass.hlsl");
    }
}
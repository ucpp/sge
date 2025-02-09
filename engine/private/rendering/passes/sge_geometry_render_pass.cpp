#include "rendering/passes/sge_geometry_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void GeometryRenderPass::OnRender(Scene* scene)
    {
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();
        
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::AlbedoMetallic, D3D12_RESOURCE_STATE_RENDER_TARGET);
        SetTargetState(RTargetType::NormalRoughness, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_context->GetDepthBuffer()->GetResource(0)->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList.Get());

        ClearRenderTargetView({RTargetType::AlbedoMetallic, RTargetType::NormalRoughness});
        SetRenderTarget({RTargetType::AlbedoMetallic, RTargetType::NormalRoughness});
    }

    void GeometryRenderPass::OnDraw(Scene* scene)
    {
        Verify(m_context, "GeometryRenderPass::OnDraw: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for(auto& pair : scene->GetModels())
        {
            pair.second->Render(m_context->GetCommandList().Get());
        }
    }
    
    PipelineConfig GeometryRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormats({ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM })
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
            .SetSampleCount(1)
            .SetShaders("/vs_geometry_pass.hlsl", "/ps_geometry_pass.hlsl");
    }
}
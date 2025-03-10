#include "rendering/passes/sge_geometry_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void GeometryRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        commandList->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();

        m_context->GetDepthBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList.Get());
        SetTargetState(output, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(output);
        SetRenderTarget(output);
    }

    void GeometryRenderPass::OnDraw(Scene* scene)
    {
        DrawModels(scene);
    }
    
    PipelineConfig GeometryRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormats({ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM })
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
            .SetSampleCount(1);
    }
}
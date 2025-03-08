#include "rendering/passes/sge_reflection_cubemap_pass.h"
#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "data/sge_texture_manager.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void ReflectionCubemapPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        //commandList->SetPipelineState(m_pipelineState->GetPipelineState());
        //m_context->SetRootSignature(m_pipelineState->GetSignature());
    }

    void ReflectionCubemapPass::OnDraw(Scene* scene)
    {
        Verify(m_context, "ReflectionCubemapPass::OnDraw: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //commandList->DrawInstanced(36, 1, 0, 0);
    }

    PipelineConfig ReflectionCubemapPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT, false)
            .SetSampleCount(1)
            .SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE)
            .SetDepthFunc(D3D12_COMPARISON_FUNC_LESS_EQUAL);
    }
}
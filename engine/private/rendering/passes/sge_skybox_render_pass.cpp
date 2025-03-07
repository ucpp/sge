#include "rendering/passes/sge_skybox_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "data/sge_texture_manager.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void SkyboxRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();
        
        m_context->SetRootDescriptorTable(0, 0);
        
        uint32 index = TextureManager::GetCubemapIndex(scene->GetSkyboxCubeMap(), m_context->GetDevice(), m_context->GetCbvSrvUavHeap());
        m_context->GetCbvSrvUavHeap()->GetGPUHandle(index);
        m_context->GetCommandList()->SetGraphicsRootDescriptorTable(2, m_context->GetCbvSrvUavHeap()->GetGPUHandle(index));
    }

    void SkyboxRenderPass::OnDraw(Scene* scene)
    {
        Verify(m_context, "SkyboxRenderPass::OnDraw: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(36, 1, 0, 0);
    }
    
    PipelineConfig SkyboxRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT, false)
            .SetSampleCount(1)
            .SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE)
            .SetDepthFunc(D3D12_COMPARISON_FUNC_LESS_EQUAL); 
    }
}
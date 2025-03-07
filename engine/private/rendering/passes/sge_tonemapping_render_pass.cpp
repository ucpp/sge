#include "rendering/passes/sge_tonemapping_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void ToneMappingRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(input, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        SetTargetState(output, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(output);
        SetRenderTarget(output);

        m_context->SetRootDescriptorTable(0, 0);
        
        uint32 descriptionTableIndex = 2;
        for(const std::string& name : input)
        {
            BindRenderTargetSRV(name, descriptionTableIndex);
            ++descriptionTableIndex;
        }
    }

    PipelineConfig ToneMappingRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM);
    }
}
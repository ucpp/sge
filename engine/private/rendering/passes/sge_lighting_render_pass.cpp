#include "rendering/passes/sge_lighting_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void LightingRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(input, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_context->GetDepthBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        m_context->GetShadowMap()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());

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
        commandList->SetGraphicsRootDescriptorTable(descriptionTableIndex, m_context->GetDepthBuffer()->GetSRVGPUHandle());
        ++descriptionTableIndex;
        commandList->SetGraphicsRootDescriptorTable(descriptionTableIndex, m_context->GetShadowMap()->GetSRVGPUHandle());
    }

    PipelineConfig LightingRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM);
    }
}
#include "rendering/passes/sge_lighting_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void LightingRenderPass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(RTargetType::AlbedoMetallic, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        SetTargetState(RTargetType::NormalRoughness, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        SetTargetState(RTargetType::SSAOBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_context->GetDepthBuffer()->GetResource(0)->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        SetTargetState(RTargetType::LightingBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(RTargetType::LightingBuffer);
        SetRenderTarget(RTargetType::LightingBuffer);

        m_context->SetRootDescriptorTable(0, 0);
        BindRenderTargetSRV(RTargetType::AlbedoMetallic, 2);
        BindRenderTargetSRV(RTargetType::NormalRoughness, 3);
        BindRenderTargetSRV(RTargetType::SSAOBuffer, 4);
        commandList->SetGraphicsRootDescriptorTable(5, m_context->GetDepthBuffer()->GetSRVGPUHandle(0));
    }

    PipelineConfig LightingRenderPass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R16G16B16A16_FLOAT, "/ps_lighting_pass.hlsl");
    }
}
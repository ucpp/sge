#include "rendering/passes/sge_bloom_combine_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void BloomCombinePass::OnRender(Scene* scene)
    {
        auto commandList = m_context->GetCommandList();
        m_context->GetBlurBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        m_context->GetLightingBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        m_context->GetBloomBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, m_context->GetCommandList().Get());
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_context->GetBloomBuffer()->GetRTVHandle();
        commandList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);
        commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetLightingBuffer()->GetSRVGPUHandle());
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetBlurBuffer()->GetSRVGPUHandle());

        commandList->DrawInstanced(6, 1, 0, 0);
    }

    PipelineConfig BloomCombinePass::GetPipelineConfig() const
    {
        return CreateFullscreenQuadPipelineConfig(DXGI_FORMAT_R8G8B8A8_UNORM, "shaders/ps_bloom_combine_pass.hlsl");
    }
}
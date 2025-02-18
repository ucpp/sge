#include "rendering/passes/sge_forward_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void ForwardRenderPass::OnRender(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        auto commandList = m_context->GetCommandList();
        SetTargetState(input, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_context->GetShadowMap()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        m_context->GetDepthBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList.Get());
        CD3DX12_CPU_DESCRIPTOR_HANDLE depthDSV = m_context->GetDepthBuffer()->GetDSVHandle();
        commandList->ClearDepthStencilView(depthDSV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->OMSetRenderTargets(0, nullptr, false, &depthDSV);
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        SetTargetState(output, D3D12_RESOURCE_STATE_RENDER_TARGET);
        ClearRenderTargetView(output);
        SetRenderTarget(output);

        m_context->SetRootDescriptorTable(0, 0);
        uint32 descriptionTableIndex = 6;
        commandList->SetGraphicsRootDescriptorTable(descriptionTableIndex, m_context->GetShadowMap()->GetSRVGPUHandle());
        ++descriptionTableIndex;
        for(const std::string& name : input)
        {
            BindRenderTargetSRV(name, descriptionTableIndex);
            ++descriptionTableIndex;
        }
    }

    void ForwardRenderPass::OnDraw(Scene* scene)
    {
        Verify(m_context, "ForwardRenderPass::OnDraw: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
            .SetShaders("/vs_forward_pass.hlsl", "/ps_forward_pass.hlsl");
    }
}
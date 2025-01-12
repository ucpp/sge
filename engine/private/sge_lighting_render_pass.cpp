#include "sge_lighting_render_pass.h"

#include "sge_render_context.h"
#include "sge_scene.h"
#include "sge_helpers.h"

namespace SGE
{
    void LightingRenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "ForwardRenderPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        pipelineConfig.SampleCount = 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_lighting_pass.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_lighting_pass.hlsl";
        
        m_pipelineState = std::make_unique<PipelineState>();
        m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig);
    }
    
    void LightingRenderPass::Render(Scene* scene)
    {
        uint32 targetCount = m_context->GetGBuffer()->GetTargetCount();
        D3D12_RESOURCE_BARRIER barriers[3];
        for (uint32 i = 0; i < targetCount; ++i)
        {
            auto resource = m_context->GetGBuffer()->GetRenderTarget(i);
            barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(
                resource,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        }
        auto commandList = m_context->GetCommandList();
        commandList->ResourceBarrier(targetCount, barriers);

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetGBuffer()->GetSRVGPUHandle(0));
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetGBuffer()->GetSRVGPUHandle(1));
        commandList->SetGraphicsRootDescriptorTable(4, m_context->GetGBuffer()->GetSRVGPUHandle(2));

        commandList->DrawInstanced(6, 1, 0, 0);
    }
    
    void LightingRenderPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
    }
}
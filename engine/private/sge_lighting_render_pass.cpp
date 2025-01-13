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
        auto commandList = m_context->GetCommandList();
        
        uint32 targetCount = m_context->GetGBuffer()->GetTargetCount();
        std::vector<CD3DX12_RESOURCE_BARRIER> barriers;
        for(uint32 i = 0; i < targetCount; ++i)
        {
            if(m_context->GetGBuffer()->GetCurrentState(i) != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
            {
                CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                    m_context->GetGBuffer()->GetRenderTarget(i),
                    m_context->GetGBuffer()->GetCurrentState(i),
                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
                );  
                m_context->GetGBuffer()->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, i);
                barriers.push_back(barrier);
            }
        }
        /*
        if(m_context->GetDepthBuffer()->GetCurrentState(0) != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_context->GetDepthBuffer()->GetDepthBuffer(0),
                m_context->GetDepthBuffer()->GetCurrentState(0),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );  
            m_context->GetGBuffer()->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);
            barriers.push_back(barrier);
        }
        */

        if(barriers.size() > 0)
        {
            commandList->ResourceBarrier(static_cast<uint32>(barriers.size()), barriers.data());
        }

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetGBuffer()->GetSRVGPUHandle(0));
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetGBuffer()->GetSRVGPUHandle(1));
        commandList->SetGraphicsRootDescriptorTable(4, m_context->GetDepthBuffer()->GetSRVGPUHandle(0));

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
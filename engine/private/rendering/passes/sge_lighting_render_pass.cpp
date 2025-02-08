#include "rendering/passes/sge_lighting_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void LightingRenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "ForwardRenderPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilState.DepthEnable = false;
        pipelineConfig.SampleCount = 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_fullscreen_quad.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_lighting_pass.hlsl";
        
        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig, m_reloadRequested);
        }
    }
    
    void LightingRenderPass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        auto commandList = m_context->GetCommandList();
        uint32 targetCount = m_context->GetGBuffer()->GetTargetCount();
        std::vector<CD3DX12_RESOURCE_BARRIER> barriers;
        for(uint32 i = 0; i < targetCount; ++i)
        {
            m_context->GetGBuffer()->GetResource(i)->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, barriers);
        }
        
        m_context->GetDepthBuffer()->GetResource(0)->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, barriers);

        if(barriers.size() > 0)
        {
            commandList->ResourceBarrier(static_cast<uint32>(barriers.size()), barriers.data());
        }
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());


        m_context->GetLightingBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, m_context->GetCommandList().Get());
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_context->GetLightingBuffer()->GetRTVHandle();
        commandList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);
        commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
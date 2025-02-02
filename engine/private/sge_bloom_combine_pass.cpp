#include "sge_bloom_combine_pass.h"

#include "sge_render_context.h"
#include "sge_scene.h"
#include "sge_helpers.h"

namespace SGE
{
    void BloomCombinePass::Initialize(RenderContext* context)
    {
        Verify(context, "BlurPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilState.DepthEnable = false;
        pipelineConfig.SampleCount = 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_fullscreen_quad.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_bloom_combine_pass.hlsl";
        
        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig, m_reloadRequested);
        }
    }
    
    void BloomCombinePass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

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
    
    void BloomCombinePass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
    }
}
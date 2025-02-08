#include "rendering/passes/sge_brightness_extraction_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void BrightnessExtractionPass::Initialize(RenderContext* context)
    {
        Verify(context, "BrightnessExtractionPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilState.DepthEnable = false;
        pipelineConfig.SampleCount = 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_fullscreen_quad.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_brightness_extraction_pass.hlsl";
        
        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig, m_reloadRequested);
        }
    }
    
    void BrightnessExtractionPass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        auto commandList = m_context->GetCommandList();
        m_context->GetLightingBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());

        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        m_context->GetBrightnessBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, m_context->GetCommandList().Get());
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_context->GetBrightnessBuffer()->GetRTVHandle();
        commandList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);
        commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetLightingBuffer()->GetSRVGPUHandle());

        commandList->DrawInstanced(6, 1, 0, 0);
    }
    
    void BrightnessExtractionPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
    }
}
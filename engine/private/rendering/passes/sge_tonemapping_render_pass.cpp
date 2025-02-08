#include "rendering/passes/sge_tonemapping_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void ToneMappingRenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "ToneMappingRenderPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilState.DepthEnable = false;
        pipelineConfig.SampleCount = 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_fullscreen_quad.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_tonemapping_pass.hlsl";
        
        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig, m_reloadRequested);
        }
    }
    
    void ToneMappingRenderPass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        auto commandList = m_context->GetCommandList();
        m_context->GetBloomBuffer()->GetResource()->TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList.Get());
        
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget(false);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetBloomBuffer()->GetSRVGPUHandle());

        commandList->DrawInstanced(6, 1, 0, 0);
    }
    
    void ToneMappingRenderPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
    }
}
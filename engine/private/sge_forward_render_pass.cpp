#include "sge_forward_render_pass.h"

#include "sge_render_context.h"
#include "sge_scene.h"
#include "sge_helpers.h"

namespace SGE
{
    void ForwardRenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "ForwardRenderPass::Initialize: Provided render context is null.");
        m_context = context;

        PipelineConfig pipelineConfig = PipelineState::CreateDefaultConfig();
        pipelineConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        pipelineConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        pipelineConfig.SampleCount = m_context->GetRenderSettings().isMSAAEnabled ? 4 : 1;
        pipelineConfig.VertexShaderPath = "shaders/vs_forward_pass.hlsl";
        pipelineConfig.PixelShaderPath = "shaders/ps_forward_pass.hlsl";
        
        m_pipelineState = std::make_unique<PipelineState>();
        m_pipelineState->Initialize(m_context->GetD12Device().Get(), pipelineConfig);
    }
    
    void ForwardRenderPass::Render(Scene* scene)
    {
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());

        m_context->SetRenderTarget();
        m_context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->SetRootDescriptorTable(0, 0);

        for(const auto& object : scene->GetRenderableObjects())
        {
            object->Render(m_context->GetCommandList().Get());
        }
    }
    
    void ForwardRenderPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
    }
}
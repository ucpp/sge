#include "sge_renderer.h"

#include "sge_application_settings.h"
#include "sge_common_structures.h"
#include "sge_device.h"
#include "sge_model_loader.h"
#include "sge_window.h"
#include "sge_scene_data.h"
#include "sge_logger.h"
#include "sge_input.h"
#include "sge_shader_manager.h"
#include "sge_render_context.h"
#include "sge_helpers.h"

namespace SGE
{
    void Renderer::Initialize(RenderContext* context)
    {
        Verify(context, "Renderer::Initialize: Provided render context is null.");
        m_context = context;

        InitializePipelineStates();

        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::InitializePipelineStates()
    {
        PipelineConfig forwardConfig = PipelineState::CreateDefaultConfig();
        forwardConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        forwardConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        forwardConfig.SampleCount = m_context->GetRenderSettings().isMSAAEnabled ? 4 : 1;
        forwardConfig.VertexShaderPath = "shaders/vs_default.hlsl";
        forwardConfig.PixelShaderPath = "shaders/ps_default.hlsl";
        
        m_forwardPipelineState = std::make_unique<PipelineState>();
        m_forwardPipelineState->Initialize(m_context->GetD12Device().Get(), forwardConfig);

        PipelineConfig deferredConfig = PipelineState::CreateDefaultConfig();
        deferredConfig.RenderTargetFormats = 
        {
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            DXGI_FORMAT_R8G8B8A8_UNORM
        };
        deferredConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        deferredConfig.SampleCount = 1;
        deferredConfig.VertexShaderPath = "shaders/vs_deferred.hlsl";
        deferredConfig.PixelShaderPath = "shaders/ps_deferred.hlsl";
        
        m_deferredPipelineState = std::make_unique<PipelineState>();
        m_deferredPipelineState->Initialize(m_context->GetD12Device().Get(), deferredConfig);
    }

    void Renderer::Render(Scene* scene, Editor* editor)
    {
        Verify(scene, "Renderer::Render: Scene is null");
        Verify(editor, "Renderer::Render: Editor is null");
    
        editor->BuildFrame();

        m_context->ResetCommandList(GetActivePipelineState()->GetPipelineState());
        m_context->SetRootSignature(GetActivePipelineState()->GetSignature());
        m_context->BindDescriptorHeaps();
        m_context->BindViewportScissors();
        m_context->ClearRenderTargets();

        m_context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->SetRootDescriptorTable(0, 0);

        for(const auto& object : scene->GetRenderableObjects())
        {
            object->Render(m_context->GetCommandList().Get());
        }
        
        editor->Render();

        m_context->PrepareRenderTargetForPresent();
        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->PresentFrame();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::Shutdown(){}

    PipelineState* Renderer::GetActivePipelineState() const
    {
        return m_context->GetRenderSettings().isDeferredRendering ? m_deferredPipelineState.get() : m_forwardPipelineState.get();
    }
}
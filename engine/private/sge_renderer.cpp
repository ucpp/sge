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
        // Forward pass
        PipelineConfig forwardConfig = PipelineState::CreateDefaultConfig();
        forwardConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        forwardConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        forwardConfig.SampleCount = m_context->GetRenderSettings().isMSAAEnabled ? 4 : 1;
        forwardConfig.VertexShaderPath = "shaders/vs_default.hlsl";
        forwardConfig.PixelShaderPath = "shaders/ps_default.hlsl";
        
        m_forwardPipelineState = std::make_unique<PipelineState>();
        m_forwardPipelineState->Initialize(m_context->GetD12Device().Get(), forwardConfig);

        // Geometry pass
        PipelineConfig deferredConfig = PipelineState::CreateDefaultConfig();
        deferredConfig.RenderTargetFormats = 
        {
            DXGI_FORMAT_R16G16B16A16_FLOAT, // Albedo + Metallic
            DXGI_FORMAT_R10G10B10A2_UNORM, // Normal + Roughness
            DXGI_FORMAT_R32_FLOAT          // Depth
        };
        deferredConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        deferredConfig.SampleCount = 1;
        deferredConfig.VertexShaderPath = "shaders/vs_deferred.hlsl";
        deferredConfig.PixelShaderPath = "shaders/ps_deferred.hlsl";
        
        m_deferredPipelineState = std::make_unique<PipelineState>();
        m_deferredPipelineState->Initialize(m_context->GetD12Device().Get(), deferredConfig);

        // Lighting pass
        PipelineConfig lightingConfig = PipelineState::CreateDefaultConfig();
        lightingConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        lightingConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        lightingConfig.SampleCount = 1;
        lightingConfig.VertexShaderPath = "shaders/vs_lighting.hlsl";
        lightingConfig.PixelShaderPath = "shaders/ps_lighting.hlsl";
        
        m_lightingPipelineState = std::make_unique<PipelineState>();
        m_lightingPipelineState->Initialize(m_context->GetD12Device().Get(), lightingConfig);
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

        const bool isDeferred = m_context->GetRenderSettings().isDeferredRendering;

        if(isDeferred)
        {
            DeferredRendering(scene);
        }
        else
        {
            ForwardRendering(scene);
        }
        
        editor->Render();

        m_context->PrepareRenderTargetForPresent();
        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->PresentFrame();
        m_context->WaitForPreviousFrame();
    }
  
    void Renderer::ForwardRendering(Scene *scene)
    {
        m_context->SetRenderTarget();
        m_context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->SetRootDescriptorTable(0, 0);

        for(const auto& object : scene->GetRenderableObjects())
        {
            object->Render(m_context->GetCommandList().Get());
        }
    }

    void Renderer::DeferredRendering(Scene *scene)
    {
        auto commandList = m_context->GetCommandList();

        uint32 targetCount = m_context->GetGBuffer()->GetTargetCount();
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[3];
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);

        for (uint32 i = 0; i < targetCount; ++i)
        {
            rtvHandles[i] = m_context->GetGBuffer()->GetRTVHandle(i);
            commandList->ClearRenderTargetView(rtvHandles[i], m_context->GetRenderSettings().backgroundColor.data(), 0, nullptr);
        }
        commandList->OMSetRenderTargets(targetCount, rtvHandles, FALSE, &dsvHandle);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (const auto& object : scene->GetRenderableObjects())
        {
            object->Render(commandList.Get());
        }
        
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
        commandList->ResourceBarrier(targetCount, barriers);

        commandList->SetPipelineState(m_lightingPipelineState->GetPipelineState());
        m_context->SetRootSignature(m_lightingPipelineState->GetSignature());
        m_context->SetRootDescriptorTable(0, 0);
        commandList->SetGraphicsRootDescriptorTable(2, m_context->GetGBuffer()->GetSRVGPUHandle(0));
        commandList->SetGraphicsRootDescriptorTable(3, m_context->GetGBuffer()->GetSRVGPUHandle(1));
        commandList->SetGraphicsRootDescriptorTable(4, m_context->GetGBuffer()->GetSRVGPUHandle(2));

        m_context->SetRenderTarget();
        commandList->DrawInstanced(6, 1, 0, 0);
    }

    void Renderer::LightingPass()
    {
    }

    void Renderer::Shutdown(){}

    PipelineState* Renderer::GetActivePipelineState() const
    {
        return m_context->GetRenderSettings().isDeferredRendering ? m_deferredPipelineState.get() : m_forwardPipelineState.get();
    }
}
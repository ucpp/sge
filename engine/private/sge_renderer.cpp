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
        InitializeSceneBuffers();
        InitializeCamera();

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

    void Renderer::InitializeSceneBuffers()
    {
        m_sceneDataBuffer = std::make_unique<ConstantBuffer>();
        m_sceneDataBuffer->Initialize(m_context->GetD12Device().Get(), m_context->GetCbvSrvUavHeap(), sizeof(SceneData), 0);

        m_model = std::make_unique<Model>();
        *m_model = ModelLoader::LoadModel("resources/backpack/backpack.obj", m_context->GetDevice(), m_context->GetCbvSrvUavHeap(), 1);
        m_model->SetRotation({0, 180.0f, 0});
    }

    void Renderer::InitializeCamera()
    {
        Vector3 target = { 0.0f, 0.0f, 0.0f };
        m_camera.SetTarget(target);
        m_camera.SetPosition({0.0f, 0.0f, -10.0f});

        m_cameraController.SetCamera(&m_camera);
        m_cameraController.SetMoveSpeed(10.0f);
        m_cameraController.SetSensitivity(0.1f);
    }
    
    void Renderer::Update(double deltaTime)
    {   
        if(Input::Get().GetKeyDown(VK_SPACE))
        {
            m_playAnimation = !m_playAnimation;
        }

        if(m_playAnimation)
        {
            float rotationSpeed = 45.0f;
            float rotationDelta = rotationSpeed * static_cast<float>(deltaTime);
            XMFLOAT3 currentRotation = m_model->GetRotation();
            currentRotation.y += rotationDelta;
            m_model->SetRotation(currentRotation);
        }

        m_cameraController.Update(deltaTime);
    }

    void Renderer::Render(Scene* scene, Editor* editor)
    {
        if(editor)
        {
            editor->BuildFrame();
        }

        BeginFrame();
        RenderGeometryPass();
        
        if(editor)
        {
            editor->Render();
        }

        m_context->PrepareRenderTargetForPresent();
        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->PresentFrame();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::BeginFrame()
    {
        m_context->ResetCommandList(GetActivePipelineState()->GetPipelineState());
        m_context->SetRootSignature(GetActivePipelineState()->GetSignature());
        m_context->BindDescriptorHeaps();
        m_context->BindViewportScissors();

        UpdateSceneDataBuffer();
        UpdateModelBuffer();

        m_context->ClearRenderTargets();
    }

    void Renderer::RenderGeometryPass()
    {
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->SetRootDescriptorTable(1, 0);
        m_model->Render(commandList);
    }

    void Renderer::UpdateSceneDataBuffer()
    {
        m_sceneData.directionalLight.direction = { 0.2f, 0.2f, 1.0f };
        m_sceneData.directionalLight.color = { 1.0f, 1.0f, 1.0f };
        m_sceneData.directionalLight.intensity = 1.2f;
        m_sceneData.cameraPosition = m_camera.GetPosition();
        m_sceneData.fogStart = 3.0f;
        m_sceneData.fogEnd = 30.0f;
        m_sceneData.fogColor = {0.314f, 0.314f, 0.314f};
        m_sceneData.fogStrength = m_context->GetRenderSettings().isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.fogDensity = 0.1f;

        m_sceneDataBuffer->Update(&m_sceneData, sizeof(SceneData));
    }

    void Renderer::UpdateModelBuffer()
    {
        m_model->Update(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight()));
    }

    void Renderer::Shutdown() 
    {
    }

    PipelineState* Renderer::GetActivePipelineState() const
    {
        return m_context->GetRenderSettings().isDeferredRendering ? m_deferredPipelineState.get() : m_forwardPipelineState.get();
    }
}
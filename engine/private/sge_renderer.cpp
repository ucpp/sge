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

namespace SGE
{
    void Renderer::Initialize(Window* window, ApplicationSettings* settings)
    {
        m_window = window;
        m_settings = settings;
        m_frameIndex = 0;

        m_device = std::make_unique<Device>();
        m_device->Initialize(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight());

        m_viewportScissors = std::make_unique<ViewportScissors>(m_window->GetWidth(), m_window->GetHeight());
        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();

        InitializeDescriptorHeaps();
        InitializeRenderTargets();
        InitializePipelineStates();
        InitializeSceneBuffers();
        InitializeCamera();

        m_fence.Initialize(m_device.get(), 1);
        m_editor = std::make_unique<Editor>();
        m_editor->Initialize(m_window, m_device.get(), m_settings);

        m_device->GetCommandList()->Close();
        ExecuteCommandList();
        WaitForPreviousFrame();
    }

    void Renderer::InitializeDescriptorHeaps()
    {
        m_cbvSrvUavHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CbvSrvHeapCapacity, true);
        uint32 rtvNumDescriptors = BufferCount * (IsMSAA() ? 2 : 1);
        m_rtvHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvNumDescriptors);
        m_dsvHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
    }

    void Renderer::InitializeRenderTargets()
    {
        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), &m_rtvHeap, BufferCount, IsMSAA());

        m_depthBuffer = std::make_unique<DepthBuffer>();
        m_depthBuffer->Initialize(m_device.get(), &m_dsvHeap, m_window->GetWidth(), m_window->GetHeight(), 1, IsMSAA());
    }

    void Renderer::InitializePipelineStates()
    {
        PipelineConfig forwardConfig = PipelineState::CreateDefaultConfig();
        forwardConfig.RenderTargetFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
        forwardConfig.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        forwardConfig.SampleCount = IsMSAA() ? 4 : 1;
        forwardConfig.VertexShaderPath = "shaders/vs_default.hlsl";
        forwardConfig.PixelShaderPath = "shaders/ps_default.hlsl";
        
        m_forwardPipelineState = std::make_unique<PipelineState>();
        m_forwardPipelineState->Initialize(m_device->GetDevice().Get(), forwardConfig);

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
        m_deferredPipelineState->Initialize(m_device->GetDevice().Get(), deferredConfig);
    }

    void Renderer::InitializeSceneBuffers()
    {
        m_sceneDataBuffer = std::make_unique<ConstantBuffer>();
        m_sceneDataBuffer->Initialize(m_device->GetDevice().Get(), &m_cbvSrvUavHeap, sizeof(SceneData), 0);

        m_model = std::make_unique<Model>();
        *m_model = ModelLoader::LoadModel("resources/backpack/backpack.obj", m_device.get(), &m_cbvSrvUavHeap, 1);
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

    void Renderer::Render()
    {
        m_editor->BuildImGuiFrame();

        BeginFrame();
        RenderGeometryPass();
        RenderImGuiPass();
        EndFrame();

        ExecuteCommandList();
        m_device->GetSwapChain()->Present(1, 0);

        WaitForPreviousFrame();
    }

    void Renderer::BeginFrame()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();

        m_device->GetCommandAllocator(m_frameIndex)->Reset();
        commandList->Reset(m_device->GetCommandAllocator(m_frameIndex).Get(), GetActivePipelineState()->GetPipelineState());
        commandList->SetGraphicsRootSignature(GetActivePipelineState()->GetSignature());

        ID3D12DescriptorHeap* heaps[] = { m_cbvSrvUavHeap.GetHeap().Get() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        m_viewportScissors->Bind(commandList);

        UpdateBuffers();

        ClearRenderTargets();
    }

    void Renderer::ClearRenderTargets()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex, IsMSAA());
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthBuffer->GetDSVHandle(0);

        const float clearColor[] = { 0.314f, 0.314f, 0.314f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    void Renderer::RenderGeometryPass()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetGraphicsRootDescriptorTable(1, m_cbvSrvUavHeap.GetGPUHandle(0));
        m_model->Render(commandList);
    }

    void Renderer::RenderImGuiPass()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();
        m_editor->Render(commandList);
    }

    void Renderer::EndFrame()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();

        if (IsMSAA())
        {
            m_renderTarget->Resolve(commandList, m_frameIndex);
        }

        ID3D12Resource* presentTarget = m_renderTarget->GetTarget(m_frameIndex);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(presentTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        commandList->Close();
    }

    void Renderer::UpdateBuffers()
    {
        UpdateSceneDataBuffer();
        UpdateModelBuffer();
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
        m_sceneData.fogStrength = m_settings->isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.fogDensity = 0.1f;

        m_sceneDataBuffer->Update(&m_sceneData, sizeof(SceneData));
    }

    void Renderer::UpdateModelBuffer()
    {
        m_model->Update(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_window->GetWidth(), m_window->GetHeight()));
    }

    void Renderer::WaitForPreviousFrame()
    {
        UINT64 fence = m_fence.Signal(m_device->GetCommandQueue().Get());
        m_fence.Wait(fence);

        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();
    }

    void Renderer::ExecuteCommandList()
    {
        ID3D12CommandList* ppCommandLists[] = { m_device->GetCommandList().Get() };
        m_device->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    void Renderer::Shutdown() 
    {
        m_editor->Shutdown();
        m_renderTarget->Shutdown();
    }

    void Renderer::ResizeScreen(uint32 width, uint32 height)
    {
        m_renderTarget->Shutdown();
        m_depthBuffer->Shutdown();
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        m_device->GetSwapChain()->GetDesc(&swapChainDesc);
        HRESULT hr = m_device->GetSwapChain()->ResizeBuffers(
            BufferCount, 
            width, 
            height, 
            swapChainDesc.BufferDesc.Format, 
            swapChainDesc.Flags
        );

        m_viewportScissors->Set(width, height);
        m_renderTarget->Resize(width, height);
        m_depthBuffer->Resize(width, height);
    
        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();
    }

    PipelineState* Renderer::GetActivePipelineState() const
    {
        return m_settings->isDefferedRendering ? m_deferredPipelineState.get() : m_forwardPipelineState.get();
    }
    
    bool Renderer::IsMSAA() const
    {
        return m_settings && m_settings->isMSAAEnabled && !m_settings->isDefferedRendering;
    }
}
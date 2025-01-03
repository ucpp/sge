#include "sge_renderer.h"

#include "sge_application_settings.h"
#include "sge_common_structures.h"
#include "sge_device.h"
#include "sge_model_loader.h"
#include "sge_window.h"
#include "sge_light.h"

namespace SGE
{
    void Renderer::Initialize(Window* window, ApplicationSettings* settings)
    {
        m_frameIndex = 0;
        m_window = window;
        m_settings = settings;
        m_device = std::make_unique<Device>();
        m_device->Initialize(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight());
        m_viewportScissors = std::make_unique<ViewportScissors>(m_window->GetWidth(), m_window->GetHeight());
        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();

        m_cbvSrvUavHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CbvSrvHeapCapacity, true);
        m_rtvHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BufferCount);
        m_dsvHeap.Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, BufferCount);

        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), &m_rtvHeap, BufferCount);

        m_depthBuffer = std::make_unique<DepthBuffer>();
        m_depthBuffer->Initialize(m_device.get(), &m_dsvHeap, m_window->GetWidth(), m_window->GetHeight(), BufferCount);

        m_vertexShader = std::make_unique<Shader>();
        m_pixelShader = std::make_unique<Shader>();

        m_vertexShader->Initialize("shaders/vs_default.hlsl", ShaderType::Vertex);
        m_pixelShader->Initialize("shaders/ps_default.hlsl", ShaderType::Pixel);

        m_rootSignature = std::make_unique<RootSignature>();
        m_rootSignature->Initialize(m_device->GetDevice().Get());

        m_pipelineState = std::make_unique<PipelineState>();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC standardDesc = PipelineState::CreateDefaultPSODesc();
        m_pipelineState->Initialize(m_device->GetDevice().Get(), *m_vertexShader, *m_pixelShader, *m_rootSignature, standardDesc);

        m_wireframePipelineState = std::make_unique<PipelineState>();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC descWireframe = PipelineState::CreateDefaultPSODesc();
        descWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        m_wireframePipelineState->Initialize(m_device->GetDevice().Get(), *m_vertexShader, *m_pixelShader, *m_rootSignature, descWireframe);

        m_fence.Initialize(m_device.get(), 1);

        m_lightDataBuffer = std::make_unique<ConstantBuffer>();
        m_lightDataBuffer->Initialize(m_device->GetDevice().Get(), &m_cbvSrvUavHeap, sizeof(LightData), 0);

        m_model = std::make_unique<Model>();
        *m_model = ModelLoader::LoadModel("resources/backpack/backpack.obj", m_device.get(), &m_cbvSrvUavHeap, 1);
        m_model->SetRotation({0, 180.0f, 0});

        InitializeCamera();

        m_editor = std::make_unique<Editor>();
        m_editor->Initialize(m_window, m_device.get(), m_settings);
        
        m_device->GetCommandList()->Close();
        ExecuteCommandList();
        WaitForPreviousFrame();
    }

    void Renderer::InitializeCamera()
    {
        Vector3 target = { 0.0f, 0.0f, 0.0f };
        m_camera.SetTarget(target);
        m_camera.SetPosition({0.0f, 0.0f, -10.0f});

        m_cameraController.SetCamera(&m_camera);
        m_cameraController.SetMoveSpeed(1000.0f);
        m_cameraController.SetSensitivity(0.1f);
    }
    
    void Renderer::Update(double deltaTime)
    {
        m_cameraController.Update(deltaTime);
    }

    void Renderer::Render()
    {
        m_editor->BuildImGuiFrame();

        PopulateCommandList();
        ExecuteCommandList();

        m_device->GetSwapChain()->Present(1, 0);
        WaitForPreviousFrame();
    }

    void Renderer::PopulateCommandList()
    {
        ID3D12GraphicsCommandList* commandList = m_device->GetCommandList().Get();

        m_device->GetCommandAllocator(m_frameIndex)->Reset();
        ID3D12PipelineState* activePipelineState = m_settings->wireframeMode 
        ? m_wireframePipelineState->GetPipelineState() 
        : m_pipelineState->GetPipelineState();

        commandList->Reset(m_device->GetCommandAllocator(m_frameIndex).Get(), activePipelineState);

        commandList->SetGraphicsRootSignature(m_rootSignature->GetSignature());

        m_model->Update(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_window->GetWidth(), m_window->GetHeight()), m_settings->lodLevel);

        LightData lightData = {};
        lightData.directionalLight.direction = { 1.0f, 0.5f, -1.0f };
        lightData.directionalLight.color = { 1.0f, 1.0f, 1.0f };
        lightData.directionalLight.intensity = 2.0f;
        lightData.pointLight.position = { 0.0f, 5.0f, 0.0f };
        lightData.pointLight.color = { 1.0f, 0.0f, 0.0f };
        lightData.pointLight.intensity = 1.0f;
        lightData.cameraPosition = m_camera.GetPosition();

        m_lightDataBuffer->Update(&lightData, sizeof(LightData));

        ID3D12DescriptorHeap* heaps[] = { m_cbvSrvUavHeap.GetHeap().Get() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        commandList->RSSetViewports(1, &m_viewportScissors->GetViewport());
        commandList->RSSetScissorRects(1, &m_viewportScissors->GetScissorRect());

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthBuffer->GetDSVHandle(m_frameIndex);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GetTarget(m_frameIndex), 
                                        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        const float clearColor[] = { 0.314f, 0.314f, 0.314f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        commandList->SetGraphicsRootDescriptorTable(1, m_cbvSrvUavHeap.GetGPUHandle(0));
        m_model->Render(commandList);
        m_editor->Render(commandList);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTarget->GetTarget(m_frameIndex), 
            D3D12_RESOURCE_STATE_RENDER_TARGET, 
            D3D12_RESOURCE_STATE_PRESENT));

        commandList->Close();
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
}
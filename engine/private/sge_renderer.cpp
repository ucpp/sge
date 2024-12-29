#include "sge_renderer.h"

#include "sge_helpers.h"
#include <d3dx12.h>

#include "sge_model_loader.h"
#include "sge_mesh.h"

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

        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), SwapChainBufferCount);

        m_vertexShader = std::make_unique<Shader>();
        m_pixelShader = std::make_unique<Shader>();

        m_vertexShader->Initialize(L"shaders/vs_default.hlsl", ShaderType::Vertex);
        m_pixelShader->Initialize(L"shaders/ps_default.hlsl", ShaderType::Pixel);

        m_rootSignature = std::make_unique<RootSignature>();
        m_rootSignature->Initialize(m_device->GetDevice().Get());

        m_pipelineState = std::make_unique<PipelineState>();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC standardDesc = PipelineState::CreateDefaultPSODesc();
        m_pipelineState->Initialize(m_device->GetDevice().Get(), *m_vertexShader, *m_pixelShader, *m_rootSignature, standardDesc);

        m_wireframePipelineState = std::make_unique<PipelineState>();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC descWireframe = PipelineState::CreateDefaultPSODesc();
        descWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        m_wireframePipelineState->Initialize(m_device->GetDevice().Get(), *m_vertexShader, *m_pixelShader, *m_rootSignature, descWireframe);

        m_device->GetCommandList()->Close();

        m_model = std::make_unique<Model>();
        *m_model = ModelLoader::LoadModel("resources/backpack/backpack.obj", m_device.get());

        m_modelBuffer = std::make_unique<ConstantBuffer>();
        m_modelBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        m_viewBuffer = std::make_unique<ConstantBuffer>();
        m_viewBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        m_projectionBuffer = std::make_unique<ConstantBuffer>();
        m_projectionBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        InitializeCamera();

        m_editor = std::make_unique<Editor>();
        m_editor->Initialize(m_window, m_device.get(), m_settings);

        m_fence.Initialize(m_device.get(), 1);
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

        ID3D12CommandList* ppCommandLists[] = { m_device->GetCommandList().Get() };
        m_device->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

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

        //XMMATRIX modelMatrix = XMMatrixIdentity();
        XMMATRIX modelMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
        m_modelBuffer->Update(&modelMatrix, sizeof(XMMATRIX));

        XMMATRIX viewMatrix = m_camera.GetViewMatrix();
        m_viewBuffer->Update(&viewMatrix, sizeof(XMMATRIX));

        XMMATRIX projectionMatrix = m_camera.GetProjMatrix(m_window->GetWidth(), m_window->GetHeight());
        m_projectionBuffer->Update(&projectionMatrix, sizeof(XMMATRIX));

        commandList->SetGraphicsRootConstantBufferView(0, m_modelBuffer->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(1, m_viewBuffer->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(2, m_projectionBuffer->GetGPUVirtualAddress());

        commandList->RSSetViewports(1, &m_viewportScissors->GetViewport());
        commandList->RSSetScissorRects(1, &m_viewportScissors->GetScissorRect());

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GetTarget(m_frameIndex), 
                                    D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        const float clearColor[] = { 0.314f, 0.314f, 0.314f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (const Mesh& mesh : m_model->GetMeshes())
        {
            mesh.Render(commandList);
        }

        //commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
        
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

    void Renderer::Shutdown() 
    {
        m_editor->Shutdown();
        m_renderTarget->Shutdown();
    }
}
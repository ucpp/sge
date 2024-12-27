#include "sge_renderer.h"

#include "sge_helpers.h"
#include <d3dx12.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"


namespace SGE
{
    void Renderer::Initialize(Window* window)
    {
        m_frameIndex = 0;
        m_window = window;
        m_device = std::make_unique<Device>();
        m_device->Initialize(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight());

        m_descriptorHeap = std::make_unique<DescriptorHeap>();
        m_descriptorHeap->Initialize(m_device->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

        m_viewportScissors = std::make_unique<ViewportScissors>(m_window->GetWidth(), m_window->GetHeight());
        
        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), SwapChainBufferCount);

        m_vertexShader = std::make_unique<Shader>();
        m_pixelShader = std::make_unique<Shader>();

        m_vertexShader->Initialize(L"shaders/vs_default.hlsl", ShaderType::Vertex);
        m_pixelShader->Initialize(L"shaders/ps_default.hlsl", ShaderType::Pixel);

        m_rootSignature = std::make_unique<RootSignature>();
        m_rootSignature->Initialize(m_device->GetDevice().Get());

        m_pipelineState = std::make_unique<PipelineState>();
        m_pipelineState->Initialize(m_device->GetDevice().Get(), *m_vertexShader, *m_pixelShader, *m_rootSignature);

        m_device->GetCommandList()->Close();

        float aspectRatio = static_cast<float>(m_window->GetWidth()) / static_cast<float>(m_window->GetHeight());

        std::vector<Vertex> vertices = 
        {
            { { 0.0f, 2.0f, 0.0f }, { 1.0f, 0.435f, 0.38f, 1.0f } },
            { { 2.5f, -2.0f, 0.0f }, { 1.0f, 0.435f, 0.38f, 1.0f } },
            { { -2.5f, -2.0f, 0.0f }, { 1.0f, 0.435f, 0.38f, 1.0f } },
        };

        m_vertexBuffer = std::make_unique<VertexBuffer>();
        m_vertexBuffer->Initialize(m_device.get(), vertices);

        std::vector<UINT> indices = { 2, 1, 0 };
        m_indexBuffer = std::make_unique<IndexBuffer>();
        m_indexBuffer->Initialize(m_device.get(), indices);

        m_modelBuffer = std::make_unique<ConstantBuffer>();
        m_modelBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        m_viewBuffer = std::make_unique<ConstantBuffer>();
        m_viewBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        m_projectionBuffer = std::make_unique<ConstantBuffer>();
        m_projectionBuffer->Initialize(m_device->GetDevice().Get(), sizeof(DirectX::XMMATRIX));

        InitializeCamera();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplWin32_Init(m_window->GetHandle());
        ImGui_ImplDX12_Init(
            m_device->GetDevice().Get(),
            SwapChainBufferCount,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            m_descriptorHeap->GetHeap().Get(),
            m_descriptorHeap->GetCPUHandle(0),
            m_descriptorHeap->GetGPUHandle(0)
        );

        m_fence.Initialize(m_device.get(), 1);
        WaitForPreviousFrame();
    }

    void Renderer::InitializeCamera()
    {
        Vector3 target = { 0.0f, 0.0f, 0.0f };
        m_camera.SetTarget(target);

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
        BuildImGuiFrame();

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
        commandList->Reset(m_device->GetCommandAllocator(m_frameIndex).Get(), m_pipelineState->GetPipelineState());

        commandList->SetGraphicsRootSignature(m_rootSignature->GetSignature());

        XMMATRIX modelMatrix = XMMatrixIdentity();
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

        commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetView());
        commandList->IASetIndexBuffer(&m_indexBuffer->GetView());

        commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
        
        ImGui::Render();
        commandList->SetDescriptorHeaps(1, m_descriptorHeap->GetHeap().GetAddressOf());
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

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

    void Renderer::BuildImGuiFrame()
    {
        // ONLY FOR TEST IMGUI!!!
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene")) { }
                if (ImGui::MenuItem("Open Scene")) { }
                if (ImGui::MenuItem("Save Scene")) { }
                if (ImGui::MenuItem("Exit")) { }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("Documentation")) { }
                if (ImGui::MenuItem("About")) { }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void Renderer::Shutdown() 
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        m_renderTarget->Shutdown();
        m_descriptorHeap->Shutdown();
    }
}
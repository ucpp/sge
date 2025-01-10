#include "sge_render_context.h"
#include "sge_application_settings.h"
#include "sge_helpers.h"
#include "sge_window.h"

namespace SGE
{
    void RenderContext::Initialize(Window* window, ApplicationSettings* settings)
    {
        Verify(window, "RenderContext::Initialize failed: window object is null!");
        m_window = window;
        m_settings = settings;
        
        HWND hwnd = m_window->GetHandle();
        Verify(hwnd, "RenderContext::Initialize failed: HWND is null!");

        uint32 width = GetScreenWidth();
        uint32 height = GetScreenHeight();
        m_device = std::make_unique<Device>();
        m_device->Initialize(hwnd, width, height);

        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();
        m_viewportScissors = std::make_unique<ViewportScissors>(width, height);

        m_fence.Initialize(m_device.get(), 1);

        InitializeDescriptorHeaps();
        InitializeRenderTargets();
    }

    void RenderContext::InitializeDescriptorHeaps()
    {
        const uint32 cbvSrvUavNumDescriptors = CbvSrvHeapCapacity;
        const uint32 rtvNumDescriptors = GetBackBufferCount() * (GetRenderSettings().isMSAAEnabled ? 2 : 1);
        const uint32 dsvNumDescriptors = 1;

        m_cbvSrvUavHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvSrvUavNumDescriptors, true);
        m_rtvHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvNumDescriptors);
        m_dsvHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvNumDescriptors);
    }

    void RenderContext::InitializeRenderTargets()
    {
        const bool isMSAA = GetRenderSettings().isMSAAEnabled;

        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), &m_rtvHeap, GetBackBufferCount(), isMSAA);

        m_depthBuffer = std::make_unique<DepthBuffer>();
        m_depthBuffer->Initialize(m_device.get(), &m_dsvHeap, GetScreenWidth(), GetScreenHeight(), 1, isMSAA);
    }

    void RenderContext::Shutdown()
    {
        if(m_renderTarget)
        {
            m_renderTarget->Shutdown();
            m_renderTarget.reset();
        }
        
        if(m_depthBuffer)
        {
            m_depthBuffer->Shutdown();
            m_depthBuffer.reset();
        }
    }
    
    ComPtr<IDXGISwapChain3> RenderContext::GetSwapChain() const
    {
        Verify(m_device, "RenderContext::GetSwapChain failed: Device is not valid!");
        return m_device->GetSwapChain();
    }
    
    ComPtr<ID3D12Device> RenderContext::GetD12Device() const
    {
        Verify(m_device, "RenderContext::GetD12Device failed: Device is not valid!");
        return m_device->GetDevice();
    }

    ComPtr<ID3D12GraphicsCommandList> RenderContext::GetCommandList() const
    {
        Verify(m_device, "RenderContext::GetCommandList failed: Device is not valid!");
        return m_device->GetCommandList();
    }

    ComPtr<ID3D12CommandQueue> RenderContext::GetCommandQueue() const
    {
        Verify(m_device, "RenderContext::GetCommandQueue failed: Device is not valid!");
        return m_device->GetCommandQueue();
    }

    HWND RenderContext::GetWindowHandle() const
    {
        Verify(m_window, "RenderContext::GetWindowHandle failed: Window is not valid!");
        return m_window->GetHandle();
    }

    uint32 RenderContext::GetScreenWidth() const
    {
        Verify(m_window, "RenderContext::GetScreenWidth failed: Window is not valid!");
        return m_window->GetWidth();
    }

    uint32 RenderContext::GetScreenHeight() const
    {
        Verify(m_window, "RenderContext::GetScreenHeight failed: Window is not valid!");
        return m_window->GetHeight();
    }

    EditorSettings& RenderContext::GetEditorSettings() const
    {
        Verify(m_settings, "RenderContext::GetEditorSettings failed: settings are not initialized.");
        return m_settings->editor;
    }

    WindowSettings& RenderContext::GetWindowSettings() const
    {
        Verify(m_settings, "RenderContext::GetWindowSettings failed: settings are not initialized.");
        return m_settings->window;
    }

    RenderSettings& RenderContext::GetRenderSettings() const
    {
        Verify(m_settings, "RenderContext::GetRenderSettings failed: settings are not initialized.");
        return m_settings->render;
    }

    void RenderContext::ExecuteCommandList()
    {
        ID3D12CommandList* ppCommandLists[] = { GetCommandList().Get() };
        GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    void RenderContext::CloseCommandList()
    {
        GetCommandList()->Close();
    }

    void RenderContext::ResetCommandList(ID3D12PipelineState* pipelineState)
    {
        Verify(pipelineState, "RenderContext::ResetCommandList: Provided pipeline state object is null.");
        Verify(m_device, "RenderContext::ResetCommandList: Device is not initialized or invalid.");
        
        ComPtr<ID3D12CommandAllocator> allocator = m_device->GetCommandAllocator(m_frameIndex);
        Verify(allocator.Get(), "RenderContext::ResetCommandList: Failed to retrieve a valid command allocator.");

        allocator->Reset();
        GetCommandList()->Reset(allocator.Get(), pipelineState);
    }

    void RenderContext::SetRootSignature(ID3D12RootSignature* rootSignature)
    {
        GetCommandList()->SetGraphicsRootSignature(rootSignature);
    }

    void RenderContext::BindDescriptorHeaps()
    {
        ID3D12DescriptorHeap* heaps[] = { m_cbvSrvUavHeap.GetHeap().Get() };
        GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
    }

    void RenderContext::BindViewportScissors()
    {
        Verify(m_viewportScissors, "RenderContext::BindViewportScissors: Viewport Scissors is not initialized or invalid.");
        m_viewportScissors->Bind(GetCommandList().Get());
    }

    void RenderContext::WaitForPreviousFrame()
    {
        uint64 fence = m_fence.Signal(GetCommandQueue().Get());
        m_fence.Wait(fence);

        m_frameIndex = GetSwapChain()->GetCurrentBackBufferIndex();
    }

    void RenderContext::PresentFrame()
    {
        GetSwapChain()->Present(1, 0);
    }

    void RenderContext::ClearRenderTargets()
    {
        const RenderSettings& settings = GetRenderSettings();
        Verify(m_renderTarget, "RenderContext::ClearRenderTargets: Render target is not initialized or invalid.");
        Verify(m_depthBuffer, "RenderContext::ClearRenderTargets: Depth buffer is not initialized or invalid.");

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex, settings.isMSAAEnabled);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthBuffer->GetDSVHandle(0);

        GetCommandList()->ClearRenderTargetView(rtvHandle, settings.backgroundColor.data(), 0, nullptr);
        GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle); 
    }

    void RenderContext::PrepareRenderTargetForPresent()
    {
        Verify(m_renderTarget, "RenderContext::PrepareRenderTargetForPresent: Render target is not initialized or invalid.");
        if (GetRenderSettings().isMSAAEnabled)
        {
            m_renderTarget->Resolve(GetCommandList().Get(), m_frameIndex);
        }

        ID3D12Resource* presentTarget = m_renderTarget->GetTarget(m_frameIndex);
        Verify(presentTarget, "RenderContext::PrepareRenderTargetForPresent: Present target is invalid.");
        GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(presentTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    }

    void RenderContext::SetWindowSize(int32 width, int32 height)
    {
        Verify(m_window, "RenderContext::SetWindowSize failed: Window is not valid!");
        m_window->SetWindowSize(width, height);

        Verify(m_renderTarget, "RenderContext::SetWindowSize: Render target is not initialized or invalid.");
        Verify(m_depthBuffer, "RenderContext::SetWindowSize: Depth buffer is not initialized or invalid.");
        m_renderTarget->Shutdown();
        m_depthBuffer->Shutdown();

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        GetSwapChain()->GetDesc(&swapChainDesc);
        DXGI_FORMAT format = swapChainDesc.BufferDesc.Format;

        HRESULT hr = GetSwapChain()->ResizeBuffers(GetBackBufferCount(), width, height, format, swapChainDesc.Flags);
        Verify(hr, "RenderContext::SetWindowSize: GetSwapChain->ResizeBuffers failed.");

        m_viewportScissors->Set(width, height);
        m_renderTarget->Resize(width, height);
        m_depthBuffer->Resize(width, height);
    
        m_frameIndex = GetSwapChain()->GetCurrentBackBufferIndex();
    }
    
    void RenderContext::SetRootDescriptorTable(uint32 rootParameterIndex, uint32 descriptorIndex)
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE handle = m_cbvSrvUavHeap.GetGPUHandle(descriptorIndex);
        GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
    }
}
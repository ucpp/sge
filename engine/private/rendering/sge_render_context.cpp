#include "rendering/sge_render_context.h"

#include "core/sge_helpers.h"
#include "core/sge_window.h"

namespace SGE
{
    void RenderContext::Initialize(Window* window, ApplicationData* appData)
    {
        Verify(window, "RenderContext::Initialize failed: window object is null!");
        m_window = window;
        m_appicationData = appData;
        
        HWND hwnd = m_window->GetHandle();
        Verify(hwnd, "RenderContext::Initialize failed: HWND is null!");

        uint32 width = GetScreenWidth();
        uint32 height = GetScreenHeight();
        m_device = std::make_unique<Device>();
        m_device->Initialize(hwnd, width, height, appData->renderData.vSync);

        m_frameIndex = m_device->GetSwapChain()->GetCurrentBackBufferIndex();
        m_viewportScissors = std::make_unique<ViewportScissors>(width, height);

        m_fence.Initialize(m_device.get(), 1);

        InitializeDescriptorHeaps();
        InitializeRenderTargets();
    }

    void RenderContext::InitializeDescriptorHeaps()
    {
        m_cbvSrvUavHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CBV_SRV_HEAP_CAPACITY, true);
        m_rtvHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_HEAP_CAPACITY);
        m_dsvHeap.Initialize(GetD12Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_HEAP_CAPACITY);
    }

    void RenderContext::InitializeRenderTargets()
    {
        m_renderTarget = std::make_unique<RenderTarget>();
        m_renderTarget->Initialize(m_device.get(), &m_rtvHeap, BUFFER_COUNT, false);

        m_depthBuffer = std::make_unique<DepthBuffer>();
        m_depthBuffer->Initialize(m_device.get(), &m_dsvHeap, &m_cbvSrvUavHeap, GetScreenWidth(), GetScreenHeight());

        m_shadowMap = std::make_unique<DepthBuffer>();
        m_shadowMap->Initialize(m_device.get(), &m_dsvHeap, &m_cbvSrvUavHeap, GetScreenWidth(), GetScreenHeight(), 1);

        m_rtts.clear();
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

        if(m_shadowMap)
        {
            m_shadowMap->Shutdown();
            m_shadowMap.reset();
        }

        for (auto& [type, buffer] : m_rtts)
        {
            if (buffer) buffer->Shutdown();
        }
        m_rtts.clear();
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

    ApplicationData& RenderContext::GetApplicationData() const
    {
        Verify(m_appicationData, "RenderContext::GetApplicationData failed: m_appicationData are not initialized.");
        return *m_appicationData;
    }

    WindowData& RenderContext::GetWindowData() const
    {
        Verify(m_appicationData, "RenderContext::GetWindowData failed: m_appicationData are not initialized.");
        return m_appicationData->windowData;
    }

    RenderData& RenderContext::GetRenderData() const
    {
        Verify(m_appicationData, "RenderContext::GetRenderData failed: m_appicationData are not initialized.");
        return m_appicationData->renderData;
    }

    AssetsData& RenderContext::GetAssetsData() const
    {
        Verify(m_appicationData, "RenderContext::GetAssetsData failed: m_appicationData are not initialized.");
        return m_appicationData->assetsData;
    }

    SceneData& RenderContext::GetSceneData() const
    {
        Verify(m_appicationData, "RenderContext::GetSceneData failed: m_appicationData are not initialized.");
        return m_appicationData->sceneData;
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
        const uint32 syncInterval = GetRenderData().vSync ? 1 : 0;
        const uint32 presentFlags = GetRenderData().vSync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
        GetSwapChain()->Present(syncInterval, presentFlags);
    }

    void RenderContext::ClearRenderTargets()
    {
        Verify(m_renderTarget, "RenderContext::ClearRenderTargets: Render target is not initialized or invalid.");
        Verify(m_depthBuffer, "RenderContext::ClearRenderTargets: Depth buffer is not initialized or invalid.");

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex, false);
        m_renderTarget->GetResource(m_frameIndex, false)->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, GetCommandList().Get());
        
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthBuffer->GetDSVHandle();
        m_depthBuffer->GetResource()->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, GetCommandList().Get());

        GetCommandList()->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    }

    void RenderContext::SetRenderTarget(bool includeDepth)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GetRTVHandle(m_frameIndex, false);
        if(includeDepth)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthBuffer->GetDSVHandle();
            m_depthBuffer->GetResource()->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, GetCommandList().Get());
            GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle); 
        }
        else
        {
            GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
        }
    }

    void RenderContext::PrepareRenderTargetForPresent()
    {
        Verify(m_renderTarget, "RenderContext::PrepareRenderTargetForPresent: Render target is not initialized or invalid.");

        // MSAA disbled
        // m_renderTarget->Resolve(GetCommandList().Get(), m_frameIndex);
        m_renderTarget->GetResource(m_frameIndex)->TransitionState(D3D12_RESOURCE_STATE_PRESENT, GetCommandList().Get());
    }

    void RenderContext::SetWindowSize(int32 width, int32 height)
    {
        Verify(m_window, "RenderContext::SetWindowSize failed: Window is not valid!");
        m_window->SetWindowSize(width, height);

        Verify(m_renderTarget, "RenderContext::SetWindowSize: Render target is not initialized or invalid.");
        Verify(m_depthBuffer, "RenderContext::SetWindowSize: Depth buffer is not initialized or invalid.");
        m_renderTarget->Shutdown();
        m_depthBuffer->Shutdown();

        for (auto& [type, buffer] : m_rtts)
        {
            if (buffer) buffer->Shutdown();
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        GetSwapChain()->GetDesc(&swapChainDesc);
        DXGI_FORMAT format = swapChainDesc.BufferDesc.Format;

        HRESULT hr = GetSwapChain()->ResizeBuffers(BUFFER_COUNT, width, height, format, swapChainDesc.Flags);
        Verify(hr, "RenderContext::SetWindowSize: GetSwapChain->ResizeBuffers failed.");

        m_viewportScissors->Set(width, height);
        m_renderTarget->Resize(width, height);
        m_depthBuffer->Resize(width, height);
        for (auto& [type, buffer] : m_rtts)
        {
            if (buffer) buffer->Resize(width, height);
        }
    
        m_frameIndex = GetSwapChain()->GetCurrentBackBufferIndex();
    }
    
    void RenderContext::SetRootDescriptorTable(uint32 rootParameterIndex, uint32 descriptorIndex)
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE handle = m_cbvSrvUavHeap.GetGPUHandle(descriptorIndex);
        GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
    }

    RenderTargetTexture* RenderContext::GetRTT(const std::string& name)
    {
        auto it = m_rtts.find(name);
        if (it != m_rtts.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    void RenderContext::CreateRTT(const std::string& name, DXGI_FORMAT format)
    {
        const uint32 width = GetScreenWidth();
        const uint32 height = GetScreenHeight();

        if(!GetRTT(name))
        {
            const uint32 currentIndex = static_cast<uint32>(m_rtts.size());
            m_rtts[name] = std::make_unique<RenderTargetTexture>();
            m_rtts[name]->Initialize(m_device.get(), &m_rtvHeap, &m_cbvSrvUavHeap, width, height, format, currentIndex);
        }
    }
}
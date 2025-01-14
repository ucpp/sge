#ifndef _SGE_RENDER_CONTEXT_H_
#define _SGE_RENDER_CONTEXT_H_

#include "pch.h"
#include "sge_non_copyable.h"
#include "sge_device.h"
#include "sge_descriptor_heap.h"
#include "sge_render_target.h"
#include "sge_depth_buffer.h"
#include "sge_fence.h"
#include "sge_viewport_scissors.h"
#include "sge_application_settings.h"
#include "sge_gbuffer.h"

namespace SGE
{
    class RenderContext : public NonCopyable
    {
    public:
        void Initialize(class Window* window, ApplicationSettings* settings);
        void Shutdown();

        Device* GetDevice() { return m_device.get(); }
        DescriptorHeap* GetCbvSrvUavHeap() { return &m_cbvSrvUavHeap; }
        DescriptorHeap* GetRtvHeap() { return &m_rtvHeap; }
        DescriptorHeap* GetDsvHeap() { return &m_dsvHeap; }

        RenderTarget* GetRenderTarget() { return m_renderTarget.get(); }
        DepthBuffer* GetDepthBuffer() { return m_depthBuffer.get(); }
        GBuffer* GetGBuffer() { return m_gBuffer.get(); }
        Fence* GetFence() { return &m_fence; }

        uint32 GetFrameIndex() const { return m_frameIndex; }

        //uint32 GetBackBufferCount() const { return BUFFER_COUNT; }
        
        ComPtr<IDXGISwapChain3> GetSwapChain() const;
        ComPtr<ID3D12Device> GetD12Device() const;
        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
        HWND GetWindowHandle() const;

        uint32 GetScreenWidth() const;
        uint32 GetScreenHeight() const;

        EditorSettings& GetEditorSettings() const;
        WindowSettings& GetWindowSettings() const;
        RenderSettings& GetRenderSettings() const;

        void ExecuteCommandList();
        void CloseCommandList();
        void ResetCommandList(ID3D12PipelineState* pipelineState);
        void SetRootSignature(ID3D12RootSignature* rootSignature);
        void BindDescriptorHeaps();
        void BindViewportScissors();
        void WaitForPreviousFrame();
        void PresentFrame();
        void ClearRenderTargets();
        void SetRenderTarget(bool includeDepth = true);
        void PrepareRenderTargetForPresent();
        void SetWindowSize(int32 width, int32 height);
        void SetRootDescriptorTable(uint32 rootParameterIndex, uint32 descriptorIndex);

    private:
        void InitializeDescriptorHeaps();
        void InitializeRenderTargets();
        void InitializeGBuffer();

    private:
        class Window* m_window = nullptr;
        ApplicationSettings* m_settings = nullptr;

        std::unique_ptr<Device> m_device;
        std::unique_ptr<ViewportScissors> m_viewportScissors;
        std::unique_ptr<RenderTarget> m_renderTarget;
        std::unique_ptr<DepthBuffer> m_depthBuffer;
        std::unique_ptr<GBuffer> m_gBuffer;

        DescriptorHeap m_cbvSrvUavHeap;
        DescriptorHeap m_rtvHeap;
        DescriptorHeap m_dsvHeap;

        Fence m_fence;
        uint32 m_frameIndex;
    };
}

#endif // !_SGE_RENDER_CONTEXT_H_
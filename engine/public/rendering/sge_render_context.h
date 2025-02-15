#ifndef _SGE_RENDER_CONTEXT_H_
#define _SGE_RENDER_CONTEXT_H_

#include "pch.h"
#include "core/sge_non_copyable.h"
#include "core/sge_device.h"
#include "core/sge_descriptor_heap.h"
#include "rendering/sge_render_target.h"
#include "rendering/sge_depth_buffer.h"
#include "core/sge_fence.h"
#include "core/sge_viewport_scissors.h"
#include "data/sge_data_structures.h"
#include "rendering/sge_render_target_texture.h"

namespace SGE
{
    class RenderContext : public NonCopyable
    {
    public:
        void Initialize(class Window* window, ApplicationData* appData);
        void Shutdown();

        Device* GetDevice() { return m_device.get(); }
        DescriptorHeap* GetCbvSrvUavHeap() { return &m_cbvSrvUavHeap; }
        DescriptorHeap* GetRtvHeap() { return &m_rtvHeap; }
        DescriptorHeap* GetDsvHeap() { return &m_dsvHeap; }

        RenderTarget* GetRenderTarget() { return m_renderTarget.get(); }
        DepthBuffer* GetDepthBuffer() { return m_depthBuffer.get(); }
        DepthBuffer* GetShadowMap() { return m_shadowMap.get(); }

        RenderTargetTexture* GetRTT(const std::string& name);
        void CreateRTT(const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
    
        Fence* GetFence() { return &m_fence; }
        uint32 GetFrameIndex() const { return m_frameIndex; }

        ComPtr<IDXGISwapChain3> GetSwapChain() const;
        ComPtr<ID3D12Device> GetD12Device() const;
        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
        HWND GetWindowHandle() const;

        uint32 GetScreenWidth() const;
        uint32 GetScreenHeight() const;

        ApplicationData& GetApplicationData() const;
        WindowData& GetWindowData() const;
        RenderData& GetRenderData() const;
        AssetsData& GetAssetsData() const;
        SceneData& GetSceneData() const;

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

    private:
        class Window* m_window = nullptr;
        ApplicationData* m_appicationData = nullptr;

        std::unique_ptr<Device> m_device;
        std::unique_ptr<ViewportScissors> m_viewportScissors;
        std::unique_ptr<RenderTarget> m_renderTarget;
        std::unique_ptr<DepthBuffer> m_depthBuffer;
        std::unique_ptr<DepthBuffer> m_shadowMap;
        std::map<std::string, std::unique_ptr<RenderTargetTexture>> m_rtts;

        DescriptorHeap m_cbvSrvUavHeap;
        DescriptorHeap m_rtvHeap;
        DescriptorHeap m_dsvHeap;

        Fence m_fence;
        uint32 m_frameIndex;
    };
}

#endif // !_SGE_RENDER_CONTEXT_H_
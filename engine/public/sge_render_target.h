#ifndef _SGE_RENDER_TARGET_H_
#define _SGE_RENDER_TARGET_H_

#include "pch.h"

namespace SGE
{
    class RenderTarget
    {
    public:
        void Initialize(class Device* device, class DescriptorHeap* rtvHeap, uint32 bufferCount, bool isMSAAEnabled = false);
        void Resize(uint32 width, uint32 height);
        void Shutdown();
        void Resolve(ID3D12GraphicsCommandList* commandList, uint32 index);

        ID3D12Resource* GetTarget(uint32 index, bool isMSAA = false) const { return isMSAA ? m_msaaTargets[index].Get() : m_normalTargets[index].Get(); }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32 index, bool isMSAA) const;
        D3D12_RESOURCE_STATES GetCurrentState(uint32 index) const;
        void SetCurrentState(D3D12_RESOURCE_STATES state, uint32 index);
        
    private:
        void CreateRenderTargets();

    private:
        class Device* m_device = nullptr;
        class DescriptorHeap* m_rtvHeap = nullptr;
        std::vector<ComPtr<ID3D12Resource>> m_normalTargets;
        std::vector<ComPtr<ID3D12Resource>> m_msaaTargets;
        std::vector<D3D12_RESOURCE_STATES> m_states;
        uint32 m_bufferCount = 0;
        bool m_isMSAAEnabled = false;
    };
}

#endif // !_SGE_RENDER_TARGET_H_

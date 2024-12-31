#ifndef _SGE_DEVICE_H_
#define _SGE_DEVICE_H_

#include "pch.h"

namespace SGE
{
    constexpr bool UseWarpDevice = true;
    constexpr uint32 SwapChainBufferCount = 2;
    constexpr uint32 SampleCount = 1;

    class Device
    {
    public:
        void Initialize(HWND hwnd, uint32 width, uint32 height);

        ComPtr<ID3D12Device> GetDevice() const { return m_device; }
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_commandQueue; }
        ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_swapChain; }
        ComPtr<ID3D12CommandAllocator> GetCommandAllocator(uint32 index) const;
        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return m_commandList; }

    private:
        void EnableDebugLayer();
        void CreateDXGIFactory();
        void CreateGraphicsDevice();
        void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter, bool highPerformance);
        ComPtr<IDXGIAdapter1> FindAdapter(IDXGIFactory* factory, bool highPerformance);
        bool IsValidAdapter(ComPtr<IDXGIAdapter1>& adapter);

        void CreateCommandQueue();
        void CreateSwapChain(HWND hwnd, uint32 width, uint32 height);
        void CreateCommandAllocators();
        void CreateCommandList();

    private:
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<IDXGIFactory4> m_dxgiFactory;
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<ID3D12CommandAllocator> m_commandAllocators[SwapChainBufferCount];

        uint32 m_dxgiFactoryCreationFlags;
    };
}

#endif // !_SGE_DEVICE_H_
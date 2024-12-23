#ifndef _SGE_DEVICE_H_
#define _SGE_DEVICE_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi.h>

#include <wrl.h>
using namespace Microsoft::WRL;

namespace SGE
{
    constexpr bool UseWarpDevice = true;
    constexpr UINT SwapChainBufferCount = 2;
    constexpr UINT SampleCount = 1;

    class Device
    {
    public:
        void Initialize(HWND hwnd, UINT width, UINT height);

        ComPtr<ID3D12Device> GetDevice() const { return m_device; }
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_commandQueue; }
        ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_swapChain; }

    private:
        void EnableDebugLayer();
        void CreateDXGIFactory();
        void CreateGraphicsDevice();
        void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter, bool highPerformance);
        ComPtr<IDXGIAdapter1> FindAdapter(IDXGIFactory* factory, bool highPerformance);
        bool IsValidAdapter(ComPtr<IDXGIAdapter1>& adapter);

        void CreateCommandQueue();
        void CreateSwapChain(HWND hwnd, UINT width, UINT height);

    private:
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<IDXGIFactory4> m_dxgiFactory;
        UINT m_dxgiFactoryCreationFlags;
    };
}

#endif // !_SGE_DEVICE_H_
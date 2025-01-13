#include "sge_device.h"

#include "sge_helpers.h"
#include "sge_logger.h"

namespace SGE
{
    void Device::Initialize(HWND hwnd, uint32 width, uint32 height)
    {
        HRESULT hr = DXGIDeclareAdapterRemovalSupport();
        Verify(hr,"Failed to declare adapter removal support.");

        EnableDebugLayer();
        CreateDXGIFactory();
        CreateGraphicsDevice();
        CreateCommandQueue();
        CreateSwapChain(hwnd, width, height);
        CreateCommandAllocators();
        CreateCommandList();
    }

    void Device::EnableDebugLayer()
    {
        m_dxgiFactoryCreationFlags = 0;

        #if defined(_DEBUG)
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            m_dxgiFactoryCreationFlags |= DXGI_CREATE_FACTORY_DEBUG;

            ComPtr<ID3D12Debug1> debugController1;
            if (SUCCEEDED(debugController.As(&debugController1)))
            {
                debugController1->SetEnableGPUBasedValidation(TRUE);
            }
        }
        else
        {
            LOG_ERROR("Failed to initialize D3D12 debug layer.");
        }
        #endif
    }
    
    void Device::CreateDXGIFactory()
    {
        HRESULT hr = CreateDXGIFactory2(m_dxgiFactoryCreationFlags, IID_PPV_ARGS(&m_dxgiFactory));
        Verify(hr, "Failed to create DXGI factory. Please check debug layer for more details.");
    }
    
    void Device::CreateGraphicsDevice()
    {
        HRESULT hr = S_OK;

        if (USE_WARP_DEVICE)
        {
            ComPtr<IDXGIAdapter> warpAdapter;
            hr = m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            Verify(hr, "Failed to enumerate WARP adapter.");
            
            hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
            Verify(hr, "Failed to create D3D12 device with WARP adapter.");
        }
        else
        {
            ComPtr<IDXGIAdapter1> hardwareAdapter;
            GetHardwareAdapter(m_dxgiFactory.Get(), &hardwareAdapter, true);
            
            hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
            Verify(hr, "Failed to create D3D12 device with hardware adapter.");
        }
    }
    
    void Device::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter, bool highPerformance)
    {
        Verify(pFactory, "pFactory is null.");
        Verify(ppAdapter, "ppAdapter is null.");

        *ppAdapter = nullptr;
        ComPtr<IDXGIAdapter1> adapter = FindAdapter(pFactory, highPerformance);
        Verify(adapter.Get(), "Failed to find a suitable hardware adapter.");

        *ppAdapter = adapter.Detach();
    }

    ComPtr<IDXGIAdapter1> Device::FindAdapter(IDXGIFactory* factory, bool highPerformance)
    {
        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory1> factory1;
        ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory1))))
        {
            DXGI_GPU_PREFERENCE gpuPreference = highPerformance ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED;

            for (uint32 index = 0; SUCCEEDED(factory1->EnumAdapters1(index, &adapter)); ++index)
            {
                if (IsValidAdapter(adapter))
                {
                    return adapter;
                }
            }
        }

        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            DXGI_GPU_PREFERENCE gpuPreference = highPerformance ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED;

            for (uint32 index = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(index, gpuPreference, IID_PPV_ARGS(&adapter))); ++index)
            {
                if (IsValidAdapter(adapter))
                {
                    return adapter;
                }
            }
        }

        return nullptr;
    }

    bool Device::IsValidAdapter(ComPtr<IDXGIAdapter1>& adapter)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            return false;
        }

        return SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
    }
    
    void Device::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

        HRESULT hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
        Verify(hr, "Failed to create command queue.");
    }
    
    void Device::CreateSwapChain(HWND hwnd, uint32 width, uint32 height)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = BUFFER_COUNT;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
        Verify(hr, "Failed to create swap chain.");

        hr = swapChain1.As(&m_swapChain);
        Verify(hr, "Failed to cast swap chain to IDXGISwapChain3.");
    }
    
    void Device::CreateCommandAllocators()
    {
        for (uint32 i = 0; i < BUFFER_COUNT; ++i)
        {
            HRESULT hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
            Verify(hr, "Failed to create command allocator.");
        }
    }

    ComPtr<ID3D12CommandAllocator> Device::GetCommandAllocator(uint32 index) const
    {
        return m_commandAllocators[index];
    }
        
    void Device::CreateCommandList()
    {
        HRESULT hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
        Verify(hr, "Failed to create command list.");
    }
}
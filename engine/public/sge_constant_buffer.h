#ifndef _SGE_CONSTANT_BUFFER_H_
#define _SGE_CONSTANT_BUFFER_H_

#include <d3d12.h>
#include <wrl.h>
#include <memory>

using namespace Microsoft::WRL;

namespace SGE
{
    class ConstantBuffer
    {
    public:
        void Initialize(ID3D12Device* device, size_t bufferSize);
        void Update(ID3D12GraphicsCommandList* commandList, const void* data, size_t dataSize);
        ID3D12Resource* GetBuffer() const { return m_buffer.Get(); }

    private:
        ComPtr<ID3D12Resource> m_buffer;
        D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;
    };
}
#endif // _SGE_CONSTANT_BUFFER_H_

#ifndef _SGE_ROOT_SIGNATURE_H_
#define _SGE_ROOT_SIGNATURE_H_

#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace SGE
{
    class RootSignature
    {
    public:
        void Initialize(ID3D12Device* device);
        ID3D12RootSignature* GetSignature() const { return m_rootSignature.Get(); }

    private:
        ComPtr<ID3D12RootSignature> m_rootSignature;
    };
}

#endif // !_SGE_ROOT_SIGNATURE_H_

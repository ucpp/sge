#ifndef _SGE_ROOT_SIGNATURE_H_
#define _SGE_ROOT_SIGNATURE_H_

#include "pch.h"

namespace SGE
{
    class RootSignature
    {
    public:
        void Initialize(ID3D12Device* device, ID3DBlob* vertexShaderBlob, ID3DBlob* pixelShaderBlob);
        ID3D12RootSignature* GetSignature() const { return m_rootSignature.Get(); }

    private:
        void CreateRootSignature(ID3D12Device* device);
        void CreateStaticSampler();

        ComPtr<ID3D12RootSignature> m_rootSignature;
        std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_descriptorRanges;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_staticSamplers;
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC m_rootSignatureDesc;
    };
}

#endif // !_SGE_ROOT_SIGNATURE_H_

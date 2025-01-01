#include "sge_root_signature.h"
#include "sge_helpers.h"

namespace SGE
{
    void RootSignature::Initialize(ID3D12Device* device)
    {
        D3D12_ROOT_PARAMETER rootParameters[2] = {};

        D3D12_DESCRIPTOR_RANGE cbvRange = {};
        cbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        cbvRange.NumDescriptors = 1;
        cbvRange.BaseShaderRegister = 0; // b0
        cbvRange.RegisterSpace = 0;
        cbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE cbvTable = {};
        cbvTable.NumDescriptorRanges = 1;
        cbvTable.pDescriptorRanges = &cbvRange;

        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].DescriptorTable = cbvTable;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

        D3D12_DESCRIPTOR_RANGE srvRange = {};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0; // t0
        srvRange.RegisterSpace = 0;
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_DESCRIPTOR_RANGE samplerRange = {};
        samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        samplerRange.NumDescriptors = 1;
        samplerRange.BaseShaderRegister = 0; // s0
        samplerRange.RegisterSpace = 0;
        samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE srvTable = {};
        srvTable.NumDescriptorRanges = 1;
        srvTable.pDescriptorRanges = &srvRange;

        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[1].DescriptorTable = srvTable;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;

        D3D12_STATIC_SAMPLER_DESC staticSampler = {};
        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0;
        staticSampler.MaxAnisotropy = 1;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0; // s0
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        rootSignatureDesc.pStaticSamplers = &staticSampler;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> serializedRootSignature;
        ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
        Verify(hr, "Failed to serialize root signature.");

        hr = device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
        Verify(hr, "Failed to create root signature.");
    }
}

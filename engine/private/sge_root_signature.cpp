#include "sge_root_signature.h"
#include "sge_helpers.h"

namespace SGE
{
    void RootSignature::Initialize(ID3D12Device* device)
    {
        D3D12_ROOT_PARAMETER rootParameters[5] = {};

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
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_DESCRIPTOR_RANGE lightCBVRange = {};
        lightCBVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        lightCBVRange.NumDescriptors = 1;
        lightCBVRange.BaseShaderRegister = 1; // b1
        lightCBVRange.RegisterSpace = 0;
        lightCBVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE lightCBVTable = {};
        lightCBVTable.NumDescriptorRanges = 1;
        lightCBVTable.pDescriptorRanges = &lightCBVRange;

        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[1].DescriptorTable = lightCBVTable;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // diffuse (t0)
        D3D12_DESCRIPTOR_RANGE diffuseRange = {};
        diffuseRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        diffuseRange.NumDescriptors = 1;
        diffuseRange.BaseShaderRegister = 0; // t0
        diffuseRange.RegisterSpace = 0;
        diffuseRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE diffuseTable = {};
        diffuseTable.NumDescriptorRanges = 1;
        diffuseTable.pDescriptorRanges = &diffuseRange;

        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].DescriptorTable = diffuseTable;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // normal (t1)
        D3D12_DESCRIPTOR_RANGE normalRange = {};
        normalRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        normalRange.NumDescriptors = 1;
        normalRange.BaseShaderRegister = 1; // t1
        normalRange.RegisterSpace = 0;
        normalRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE normalTable = {};
        normalTable.NumDescriptorRanges = 1;
        normalTable.pDescriptorRanges = &normalRange;

        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[3].DescriptorTable = normalTable;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // specular (t2)
        D3D12_DESCRIPTOR_RANGE specularRange = {};
        specularRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        specularRange.NumDescriptors = 1;
        specularRange.BaseShaderRegister = 2; // t2
        specularRange.RegisterSpace = 0;
        specularRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_DESCRIPTOR_TABLE specularTable = {};
        specularTable.NumDescriptorRanges = 1;
        specularTable.pDescriptorRanges = &specularRange;

        rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[4].DescriptorTable = specularTable;
        rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.pStaticSamplers = &staticSampler;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> serializedRootSignature;
        ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
        Verify(hr, "Failed to serialize root signature.");

        hr = device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
        Verify(hr, "Failed to create root signature.");
    }
}

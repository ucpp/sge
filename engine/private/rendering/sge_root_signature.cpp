#include "rendering/sge_root_signature.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void RootSignature::Initialize(ID3D12Device* device, ID3DBlob* vertexShaderBlob, ID3DBlob* pixelShaderBlob)
    {
        CreateRootSignature(device);
    }
    
    void RootSignature::CreateRootSignature(ID3D12Device *device)
    {
        m_descriptorRanges.resize(8);
        m_rootParameters.resize(8);

        m_descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // b0
        m_rootParameters[0].InitAsDescriptorTable(1, &m_descriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);

        m_descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // b1
        m_rootParameters[1].InitAsDescriptorTable(1, &m_descriptorRanges[1], D3D12_SHADER_VISIBILITY_ALL);

        m_descriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t0
        m_rootParameters[2].InitAsDescriptorTable(1, &m_descriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);

        m_descriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t1
        m_rootParameters[3].InitAsDescriptorTable(1, &m_descriptorRanges[3], D3D12_SHADER_VISIBILITY_PIXEL);

        m_descriptorRanges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t2
        m_rootParameters[4].InitAsDescriptorTable(1, &m_descriptorRanges[4], D3D12_SHADER_VISIBILITY_PIXEL);  

        m_descriptorRanges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t3
        m_rootParameters[5].InitAsDescriptorTable(1, &m_descriptorRanges[5], D3D12_SHADER_VISIBILITY_PIXEL);  

        m_descriptorRanges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t4
        m_rootParameters[6].InitAsDescriptorTable(1, &m_descriptorRanges[6], D3D12_SHADER_VISIBILITY_PIXEL);  

        m_descriptorRanges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE); // t5
        m_rootParameters[7].InitAsDescriptorTable(1, &m_descriptorRanges[7], D3D12_SHADER_VISIBILITY_PIXEL); 

        m_staticSamplers.clear();
        CreateWrapSampler();
        CreateClampSampler();

        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        uint32 numParameters = static_cast<uint32>(m_rootParameters.size());
        uint32 numStaticSamplers = static_cast<uint32>(m_staticSamplers.size());
        m_rootSignatureDesc.Init_1_1(numParameters, m_rootParameters.data(), numStaticSamplers , m_staticSamplers.data(),
                                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr = D3DX12SerializeVersionedRootSignature(&m_rootSignatureDesc, featureData.HighestVersion, &signature, &error);
        Verify(hr, "Failed to serialize root signature.");

        hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
        Verify(hr, "Failed to create root signature.");
    }

    void RootSignature::CreateWrapSampler()
    {
        CD3DX12_STATIC_SAMPLER_DESC staticSampler = {};
        staticSampler.Filter = D3D12_FILTER_ANISOTROPIC;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0;
        staticSampler.MaxAnisotropy = 16;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0; // s0
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        m_staticSamplers.push_back(staticSampler);
    }

    void RootSignature::CreateClampSampler()
    {
        CD3DX12_STATIC_SAMPLER_DESC clampSampler = {};
        clampSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        clampSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSampler.MipLODBias = 0;
        clampSampler.MaxAnisotropy = 1;
        clampSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        clampSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        clampSampler.MinLOD = 0.0f;
        clampSampler.MaxLOD = D3D12_FLOAT32_MAX;
        clampSampler.ShaderRegister = 1;
        clampSampler.RegisterSpace = 0;
        clampSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        m_staticSamplers.push_back(clampSampler);
    }
}

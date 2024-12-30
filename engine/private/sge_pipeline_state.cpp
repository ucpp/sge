#include "sge_pipeline_state.h"

#include "sge_helpers.h"

namespace SGE
{
    void PipelineState::Initialize(
        ID3D12Device* device, 
        const Shader& vertexShader, 
        const Shader& pixelShader, 
        const RootSignature& rootSignature,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
    {
        m_rootSignature = rootSignature;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = psoDesc;
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        desc.pRootSignature = rootSignature.GetSignature();
        desc.VS = vertexShader.GetShaderBytecode();
        desc.PS = pixelShader.GetShaderBytecode();

        HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
        Verify(hr, "Failed to create graphics pipeline state.");
    }
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineState::CreateDefaultPSODesc()
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        return psoDesc;
    }
}
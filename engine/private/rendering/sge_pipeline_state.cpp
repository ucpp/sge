#include "rendering/sge_pipeline_state.h"

#include "core/sge_helpers.h"
#include "data/sge_shader.h"
#include "data/sge_shader_manager.h"

namespace SGE
{
    void PipelineState::Initialize(ID3D12Device* device, const PipelineConfig& config, bool reload)
    {       
        if (reload)
        {
            m_pipelineState.Reset();
        }

        const Shader& vertexShader = ShaderManager::GetShader(config.VertexShaderPath, ShaderType::Vertex, reload);
        const Shader& pixelShader = ShaderManager::GetShader(config.PixelShaderPath, ShaderType::Pixel, reload);
        m_rootSignature.Initialize(device, vertexShader.GetBlob(), pixelShader.GetBlob());

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.InputLayout = config.InputLayout;
        desc.pRootSignature = m_rootSignature.GetSignature();
        desc.VS = vertexShader.GetShaderBytecode();
        desc.PS = pixelShader.GetShaderBytecode();

        desc.RasterizerState = config.RasterizerState;
        desc.BlendState = config.BlendState;
        desc.DepthStencilState = config.DepthStencilState;
        desc.SampleMask = UINT_MAX;

        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.NumRenderTargets = static_cast<uint32>(config.RenderTargetFormats.size());

        for (size_t i = 0; i < config.RenderTargetFormats.size(); ++i)
        {
            desc.RTVFormats[i] = config.RenderTargetFormats[i];
        }

        desc.DSVFormat = config.DepthStencilFormat;
        desc.SampleDesc.Count = config.SampleCount;
        desc.SampleDesc.Quality = 0;

        HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
        Verify(hr, "Failed to create graphics pipeline state.");
    }

    PipelineConfig PipelineState::CreateDefaultConfig()
    {
        PipelineConfig config{};

        config.RenderTargetFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);

        static const D3D12_INPUT_ELEMENT_DESC defaultInputLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        config.InputLayout = { defaultInputLayout, _countof(defaultInputLayout) };

        config.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        config.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        config.DepthStencilState.DepthEnable = TRUE;
        config.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        
        return config;
    }
}
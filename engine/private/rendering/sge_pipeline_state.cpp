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

        m_rootSignature.Initialize(device);
        if (!config.ComputeShaderPath.empty())
        {
            InitializeComputePipeline(device, config);
        }
        else
        {
            InitializeGraphicsPipeline(device, config);
        }
    }

    void PipelineState::InitializeShader(const std::string& shaderPath, ShaderType type, bool reload, D3D12_SHADER_BYTECODE& shaderBytecode)
    {
        if (!shaderPath.empty())
        {
            const Shader& shader = ShaderManager::GetShader(shaderPath, type, reload);
            shaderBytecode = shader.GetShaderBytecode();
        }
    }

    void PipelineState::InitializeGraphicsPipeline(ID3D12Device* device, const PipelineConfig& config)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.InputLayout = config.InputLayout;
        desc.pRootSignature = m_rootSignature.GetSignature();

        InitializeShader(config.VertexShaderPath, ShaderType::Vertex, false, desc.VS);
        InitializeShader(config.PixelShaderPath, ShaderType::Pixel, false, desc.PS);
        InitializeShader(config.GeometryShaderPath, ShaderType::Geometry, false, desc.GS);

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

    void PipelineState::InitializeComputePipeline(ID3D12Device* device, const PipelineConfig& config)
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
        desc.pRootSignature = m_rootSignature.GetSignature();

        InitializeShader(config.ComputeShaderPath, ShaderType::Compute, false, desc.CS);

        HRESULT hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
        Verify(hr, "Failed to create compute pipeline state.");
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
            { "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        config.InputLayout = { defaultInputLayout, _countof(defaultInputLayout) };

        config.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        config.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        config.DepthStencilState.DepthEnable = TRUE;
        config.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

        return config;
    }
}
#ifndef _SGE_PIPELINE_STATE_H_
#define _SGE_PIPELINE_STATE_H_

#include "pch.h"
#include "data/sge_shader.h"
#include "rendering/sge_root_signature.h"

namespace SGE
{
    struct PipelineConfig
    {
        std::vector<DXGI_FORMAT> RenderTargetFormats;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
        D3D12_INPUT_LAYOUT_DESC InputLayout = {};

        CD3DX12_RASTERIZER_DESC RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        CD3DX12_BLEND_DESC BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        D3D12_DEPTH_STENCIL_DESC DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        uint32 SampleCount = 1;

        std::string VertexShaderPath;
        std::string PixelShaderPath;
        std::string ComputeShaderPath;
        std::string GeometryShaderPath;

        PipelineConfig& SetRenderTargetFormat(DXGI_FORMAT format)
        {
            RenderTargetFormats = { format };
            return *this;
        }

        PipelineConfig& SetRasterizerState(D3D12_FILL_MODE fillMode, D3D12_CULL_MODE cullMode)
        {
            RasterizerState.FillMode = fillMode;
            RasterizerState.CullMode = cullMode;
            return *this;
        }

        PipelineConfig& SetDepthFunc(D3D12_COMPARISON_FUNC depthFunc)
        {
            DepthStencilState.DepthFunc = depthFunc;
            return *this;
        }
    
        PipelineConfig& SetRenderTargetFormats(std::initializer_list<DXGI_FORMAT> formats)
        {
            RenderTargetFormats = formats;
            return *this;
        }
    
        PipelineConfig& SetDepthStencilFormat(DXGI_FORMAT format, bool depthEnable = true)
        {
            DepthStencilFormat = format;
            DepthStencilState.DepthEnable = depthEnable;
            return *this;
        }
    
        PipelineConfig& SetSampleCount(uint32_t count)
        {
            SampleCount = count;
            return *this;
        }

        PipelineConfig& SetVertexShaderPath(const std::string& path)
        {
            if(!path.empty())
            {
                VertexShaderPath = std::string(SHADERS_DIRECTORY) + path;
            }
            return *this;
        }

        PipelineConfig& SetPixelShaderPath(const std::string& path)
        {
            if(!path.empty())
            {
                PixelShaderPath = std::string(SHADERS_DIRECTORY) + path;
            }
            return *this;
        }

        PipelineConfig& SetComputeShaderPath(const std::string& path)
        {
            if(!path.empty())
            {
                ComputeShaderPath = std::string(SHADERS_DIRECTORY) + path;
            }
            return *this;
        }

        PipelineConfig& SetGeometryShaderPath(const std::string& path)
        {
            if(!path.empty())
            {
                GeometryShaderPath = std::string(SHADERS_DIRECTORY) + path;
            }
            return *this;
        }
    };

    class PipelineState
    {
    public:
        void Initialize(ID3D12Device* device, const PipelineConfig& config, bool reload = false);
        ID3D12PipelineState* GetPipelineState() const { return m_pipelineState.Get(); }
        ID3D12RootSignature* GetSignature() const { return m_rootSignature.GetSignature(); }

        static PipelineConfig CreateDefaultConfig();

    private:
        void InitializeShader(const std::string& shaderPath, ShaderType type, bool reload, D3D12_SHADER_BYTECODE& shaderBytecode);
        void InitializeGraphicsPipeline(ID3D12Device* device, const PipelineConfig& config);
        void InitializeComputePipeline(ID3D12Device* device, const PipelineConfig& config);

        ComPtr<ID3D12PipelineState> m_pipelineState;
        RootSignature m_rootSignature;
    };
}

#endif // !_SGE_PIPELINE_STATE_H_

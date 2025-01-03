#ifndef _SGE_PIPELINE_STATE_H_
#define _SGE_PIPELINE_STATE_H_

#include "pch.h"
#include "sge_shader.h"
#include "sge_root_signature.h"

namespace SGE
{
    class PipelineState
    {
    public:

        void Initialize(
            ID3D12Device* device, 
            const Shader& vertexShader, 
            const Shader& pixelShader, 
            const RootSignature& rootSignature,
            const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
        
        ID3D12PipelineState* GetPipelineState() const { return m_pipelineState.Get(); }
        const RootSignature& GetRootSignature() const { return m_rootSignature; }

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateDefaultPSODesc();

    private:
        ComPtr<ID3D12PipelineState> m_pipelineState;
        RootSignature m_rootSignature;
    };
}

#endif // !_SGE_PIPELINE_STATE_H_

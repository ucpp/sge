#ifndef _SGE_PIPELINE_STATE_H_
#define _SGE_PIPELINE_STATE_H_

#include <d3d12.h>
#include <wrl.h>
#include "sge_shader.h"
#include "sge_root_signature.h"

using namespace Microsoft::WRL;

namespace SGE
{
    class PipelineState
    {
    public:
        void Initialize(ID3D12Device* device, const Shader& vertexShader, const Shader& pixelShader, const RootSignature& rootSignature);
        ID3D12PipelineState* GetPipelineState() const { return m_pipelineState.Get(); }
        const RootSignature& GetRootSignature() const { return m_rootSignature; }

    private:
        ComPtr<ID3D12PipelineState> m_pipelineState;
        RootSignature m_rootSignature;
    };
}

#endif // !_SGE_PIPELINE_STATE_H_

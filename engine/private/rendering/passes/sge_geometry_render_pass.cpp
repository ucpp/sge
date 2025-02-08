#include "rendering/passes/sge_geometry_render_pass.h"

#include "rendering/sge_render_context.h"
#include "data/sge_scene.h"
#include "core/sge_helpers.h"

namespace SGE
{    
    void GeometryRenderPass::OnRender(Scene* scene)
    {
        m_context->GetCommandList()->SetPipelineState(m_pipelineState->GetPipelineState());
        m_context->SetRootSignature(m_pipelineState->GetSignature());
        m_context->SetRenderTarget();
        
        auto commandList = m_context->GetCommandList();
        uint32 targetCount = m_context->GetGBuffer()->GetTargetCount();
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

        std::vector<CD3DX12_RESOURCE_BARRIER> barriers;
        for(uint32 i = 0; i < targetCount; ++i)
        {
            m_context->GetGBuffer()->GetResource(i)->TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, barriers);
        }

        m_context->GetDepthBuffer()->GetResource(0)->TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE, barriers);

        if(barriers.size() > 0)
        {
            commandList->ResourceBarrier(static_cast<uint32>(barriers.size()), barriers.data());
        }

        for (uint32 i = 0; i < targetCount; ++i)
        {
            rtvHandles[i] = m_context->GetGBuffer()->GetRTVHandle(i);
            commandList->ClearRenderTargetView(rtvHandles[i], CLEAR_COLOR, 0, nullptr);
        }
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);
        commandList->OMSetRenderTargets(targetCount, rtvHandles, false, &dsvHandle);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for(auto& pair : scene->GetModels())
        {
            pair.second->Render(m_context->GetCommandList().Get());
        }
    }
    
    PipelineConfig GeometryRenderPass::GetPipelineConfig() const
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormats({ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM })
            .SetDepthStencilFormat(DXGI_FORMAT_D32_FLOAT)
            .SetSampleCount(1)
            .SetShaders("shaders/vs_geometry_pass.hlsl", "shaders/ps_geometry_pass.hlsl");
    }
}
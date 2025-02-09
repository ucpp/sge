#include "rendering/passes/sge_render_pass.h"

#include "rendering/sge_render_context.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void RenderPass::Initialize(RenderContext* context)
    {
        Verify(context, "RenderPass::Initialize: Provided render context is null.");
        m_context = context;

        OnInitialize(context);

        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), GetPipelineConfig(), m_reloadRequested);
        }
    }

    void RenderPass::Render(Scene* scene)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        OnRender(scene);
        OnDraw(scene);
    }

    void RenderPass::Shutdown()
    {
        if(m_pipelineState)
        {
            m_pipelineState.reset();
        }
        OnShutdown();
    }

    PipelineConfig RenderPass::CreateFullscreenQuadPipelineConfig(DXGI_FORMAT renderTargetFormat, const std::string& pixelShaderPath)
    {
        return PipelineState::CreateDefaultConfig()
            .SetRenderTargetFormat(renderTargetFormat)
            .SetDepthStencilFormat(DXGI_FORMAT_UNKNOWN, false)
            .SetSampleCount(1)
            .SetShaders("/vs_fullscreen_quad.hlsl", pixelShaderPath);
    }

    void RenderPass::SetTargetState(RTargetType type, D3D12_RESOURCE_STATES state)
    {
        Verify(m_context, "RenderPass::SetTargetState: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        
        RenderTargetTexture* rtt = m_context->GetRTT(type);
        Verify(rtt, "RenderPass::SetTargetState: Render target texture is null.");

        rtt->GetResource()->TransitionState(state, commandList);
    }

    void RenderPass::ClearRenderTargetView(RTargetType type)
    {
        ClearRenderTargetView(std::vector<RTargetType>{ type });
    }

    void RenderPass::ClearRenderTargetView(const std::vector<RTargetType>& types)
    {
        Verify(m_context, "RenderPass::ClearRenderTargetView: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        for (RTargetType type : types)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_context->GetRTT(type)->GetRTVHandle();
            commandList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        }
    }

    void RenderPass::SetRenderTarget(RTargetType type)
    {
        SetRenderTarget(std::vector<RTargetType>{ type });
    }

    void RenderPass::SetRenderTarget(const std::vector<RTargetType>& types)
    {
        Verify(m_context, "RenderPass::SetRenderTarget: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
        rtvHandles.reserve(types.size());

        for (RTargetType type : types)
        {
            rtvHandles.push_back(m_context->GetRTT(type)->GetRTVHandle());
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle(0);
        commandList->OMSetRenderTargets(static_cast<uint32>(rtvHandles.size()), rtvHandles.data(), false, &dsvHandle);
    }

    void RenderPass::BindRenderTargetSRV(RTargetType type, uint32 descIndex)
    {
        Verify(m_context, "RenderPass::BindRenderTargetSRV: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        commandList->SetGraphicsRootDescriptorTable(descIndex, m_context->GetRTT(type)->GetSRVGPUHandle());
    }

    void RenderPass::OnDraw(Scene* scene)
    {
        DrawQuad();
    }

    void RenderPass::DrawQuad()
    {
        Verify(m_context, "RenderPass::DrawQuad: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(6, 1, 0, 0);
    }
}

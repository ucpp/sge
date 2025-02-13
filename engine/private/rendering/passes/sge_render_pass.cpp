#include "rendering/passes/sge_render_pass.h"

#include "rendering/sge_render_context.h"
#include "core/sge_helpers.h"

namespace SGE
{
    void RenderPass::Initialize(RenderContext* context, const std::string& passName)
    {
        Verify(context, "RenderPass::Initialize: Provided render context is null.");
        m_context = context;
        m_name = passName;

        OnInitialize(context);

        if (!m_pipelineState || m_reloadRequested)
        {
            m_pipelineState = std::make_unique<PipelineState>();
            m_pipelineState->Initialize(m_context->GetD12Device().Get(), GetPipelineConfig(), m_reloadRequested);
        }
    }

    void RenderPass::Render(Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output)
    {
        if (m_reloadRequested)
        {
            Initialize(m_context);
            m_reloadRequested = false;
        }

        SCOPED_EVENT_GPU(m_context->GetCommandList().Get(), m_name.c_str());

        OnRender(scene, input, output);
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

    void RenderPass::SetTargetState(const std::vector<std::string>& names, D3D12_RESOURCE_STATES state)
    {
        Verify(m_context, "RenderPass::SetTargetState: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();
        for (const std::string& name : names)
        {
            RenderTargetTexture* rtt = m_context->GetRTT(name);
            Verify(rtt, "RenderPass::SetTargetState: Render target texture is null.");
            rtt->GetResource()->TransitionState(state, commandList);
        }
    }

    void RenderPass::SetTargetState(const std::string& name, D3D12_RESOURCE_STATES state)
    {
        SetTargetState(std::vector<std::string>{ name }, state);
    }

    void RenderPass::ClearRenderTargetView(const std::string& name)
    {
        ClearRenderTargetView(std::vector<std::string>{ name });
    }

    void RenderPass::ClearRenderTargetView(const std::vector<std::string>& names)
    {
        Verify(m_context, "RenderPass::ClearRenderTargetView: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        for (const std::string& name : names)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_context->GetRTT(name)->GetRTVHandle();
            commandList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
        }
    }

    void RenderPass::SetRenderTarget(const std::string& name)
    {
        SetRenderTarget(std::vector<std::string>{ name });
    }

    void RenderPass::SetRenderTarget(const std::vector<std::string>& names)
    {
        Verify(m_context, "RenderPass::SetRenderTarget: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
        rtvHandles.reserve(names.size());

        for (const std::string& name : names)
        {
            rtvHandles.push_back(m_context->GetRTT(name)->GetRTVHandle());
        }

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_context->GetDepthBuffer()->GetDSVHandle();
        commandList->OMSetRenderTargets(static_cast<uint32>(rtvHandles.size()), rtvHandles.data(), false, &dsvHandle);
    }

    void RenderPass::BindRenderTargetSRV(const std::string& name, uint32 descIndex)
    {
        Verify(m_context, "RenderPass::BindRenderTargetSRV: Render context is null.");
        ID3D12GraphicsCommandList* commandList = m_context->GetCommandList().Get();

        commandList->SetGraphicsRootDescriptorTable(descIndex, m_context->GetRTT(name)->GetSRVGPUHandle());
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

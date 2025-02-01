#include "sge_renderer.h"

#include "sge_render_context.h"
#include "sge_forward_render_pass.h"
#include "sge_geometry_render_pass.h"
#include "sge_lighting_render_pass.h"
#include "sge_tonemapping_render_pass.h"
#include "sge_helpers.h"
#include "sge_scoped_event.h"

namespace SGE
{
    void Renderer::Initialize(RenderContext* context)
    {
        Verify(context, "Renderer::Initialize: Provided render context is null.");
        m_context = context;

        m_forwardPass = std::make_unique<ForwardRenderPass>();
        m_forwardPass->Initialize(m_context);

        m_geometryPass = std::make_unique<GeometryRenderPass>();
        m_geometryPass->Initialize(m_context);

        m_lightingPass = std::make_unique<LightingRenderPass>();
        m_lightingPass->Initialize(m_context);

        m_tonemappingPass = std::make_unique<ToneMappingRenderPass>();
        m_tonemappingPass->Initialize(m_context);

        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::Render(Scene* scene, Editor* editor)
    {
        Verify(scene, "Renderer::Render: Scene is null");
        Verify(editor, "Renderer::Render: Editor is null");

        editor->BuildFrame();

        m_context->ResetCommandList(nullptr);
        m_context->BindDescriptorHeaps();
        m_context->BindViewportScissors();
        m_context->ClearRenderTargets();

        if(m_context->GetRenderData().isDeferredRendering)
        {
            m_geometryPass->Render(scene);
            m_lightingPass->Render(scene);
            m_tonemappingPass->Render(scene);
        }
        else
        {
            m_forwardPass->Render(scene);
        }
        
        editor->Render();

        m_context->PrepareRenderTargetForPresent();
        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->PresentFrame();
        m_context->WaitForPreviousFrame();
    }
  
    void Renderer::Shutdown()
    {
        if(m_forwardPass)
        {
            m_forwardPass->Shutdown();
            m_forwardPass.reset();
        }

        if(m_geometryPass)
        {
            m_geometryPass->Shutdown();
            m_geometryPass.reset();
        }

        if(m_lightingPass)
        {
            m_lightingPass->Shutdown();
            m_lightingPass.reset();
        }

        if(m_tonemappingPass)
        {
            m_tonemappingPass->Shutdown();
            m_tonemappingPass.reset();
        }
    }
    
    void Renderer::ReloadShaders()
    {
        m_context->WaitForPreviousFrame();
        m_forwardPass->Reload();
        m_geometryPass->Reload();
        m_lightingPass->Reload();
        m_tonemappingPass->Reload();
    }
}
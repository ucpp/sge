#include "rendering/sge_renderer.h"

#include "rendering/sge_render_context.h"
#include "rendering/passes/sge_render_pass.h"
#include "rendering/passes/sge_forward_render_pass.h"
#include "rendering/passes/sge_geometry_render_pass.h"
#include "rendering/passes/sge_lighting_render_pass.h"
#include "rendering/passes/sge_tonemapping_render_pass.h"
#include "rendering/passes/sge_brightness_extraction_pass.h"
#include "rendering/passes/sge_blur_pass.h"
#include "rendering/passes/sge_bloom_combine_pass.h"
#include "core/sge_helpers.h"
#include "core/sge_scoped_event.h"

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

        m_brightnesPass = std::make_unique<BrightnessExtractionPass>();
        m_brightnesPass->Initialize(m_context);

        m_blurPass = std::make_unique<BlurPass>();
        m_blurPass->Initialize(m_context);

        m_bloomCombinePass = std::make_unique<BloomCombinePass>();
        m_bloomCombinePass->Initialize(m_context);

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
            m_brightnesPass->Render(scene);
            m_blurPass->Render(scene);
            m_bloomCombinePass->Render(scene);
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

        if(m_brightnesPass)
        {
            m_brightnesPass->Shutdown();
            m_brightnesPass.reset();
        }

        if(m_blurPass)
        {
            m_blurPass->Shutdown();
            m_blurPass.reset();
        }

        if(m_bloomCombinePass)
        {
            m_bloomCombinePass->Shutdown();
            m_bloomCombinePass.reset();
        }
    }
    
    void Renderer::ReloadShaders()
    {
        m_context->WaitForPreviousFrame();
        m_forwardPass->Reload();
        m_geometryPass->Reload();
        m_lightingPass->Reload();
        m_tonemappingPass->Reload();
        m_brightnesPass->Reload();
        m_blurPass->Reload();
        m_bloomCombinePass->Reload();
    }
}
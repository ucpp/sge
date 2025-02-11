#include "rendering/sge_renderer.h"

#include "rendering/sge_render_context.h"
#include "rendering/passes/sge_render_pass_factory.h"
#include "core/sge_helpers.h"
#include "core/sge_scoped_event.h"

namespace SGE
{
    void Renderer::Initialize(RenderContext* context)
    {
        Verify(context, "Renderer::Initialize: Provided render context is null.");
        m_context = context;

        RegisterRenderPasses();
        m_renderPasses.clear();

        const RenderData& data = m_context->GetRenderData();

        const std::unordered_set<std::string> passes = data.GetAllPassNames();
        for (const auto& name : passes)
        {
            InitializeRenderPass(name, context);
        }

        const std::unordered_set<std::string> rtts = data.GetAllOutputs();
        for (const auto& name : rtts)
        {
            m_context->CreateRTT(name);
        }

        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::InitializeRenderPass(const std::string& name, RenderContext* context)
    {
        auto& factory = RenderPassFactory::Get();
        if (m_renderPasses.find(name) == m_renderPasses.end())
        {
            m_renderPasses[name] = factory.Create(name);
            m_renderPasses[name]->Initialize(context);
        }
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

        const RenderData& data = m_context->GetRenderData();
        const RenderTechnique technique = data.technique;

        if (technique == RenderTechnique::Deferred)
        {
            RenderPasses(data.deferredPasses, scene);
        }
        else if (technique == RenderTechnique::Forward)
        {
            RenderPasses(data.forwardPasses, scene);
        }

        editor->Render();

        m_context->PrepareRenderTargetForPresent();
        m_context->CloseCommandList();
        m_context->ExecuteCommandList();
        m_context->PresentFrame();
        m_context->WaitForPreviousFrame();
    }

    void Renderer::RenderPasses(const std::vector<RenderPassData>& passes, Scene* scene)
    {
        for (const auto& passData : passes)
        {
            if (m_renderPasses.find(passData.name) != m_renderPasses.end())
            {
                const std::vector<std::string>& input = passData.input;
                const std::vector<std::string>& output = passData.output;
                m_renderPasses[passData.name]->Render(scene, input, output);
            }
        }
    }

    void Renderer::Shutdown()
    {
        for (auto& [name, pass] : m_renderPasses)
        {
            pass->Shutdown();
            pass.reset();
        }
        m_renderPasses.clear();
    }

    void Renderer::ReloadShaders()
    {
        m_context->WaitForPreviousFrame();

        for (auto& [name, pass] : m_renderPasses)
        {
            pass->Reload();
        }
    }
}
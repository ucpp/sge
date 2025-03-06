#ifndef _SGE_RENDERER_H_
#define _SGE_RENDERER_H_

#include "pch.h"

#include "core/sge_non_copyable.h"
#include "rendering/passes/sge_render_pass.h"
#include "data/sge_scene.h"
#include "rendering/sge_editor.h"

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(class RenderContext* context);
        void Render(Scene* scene, Editor* editor);
        void Shutdown();

        void ReloadShaders();

    private:
        void InitializeRenderPass(const std::string& name, const RenderPassData& passData, RenderContext* context);
        void RenderPasses(const std::vector<RenderPassData>& passes, Scene* scene);

        class RenderContext* m_context = nullptr;
        std::unordered_map<std::string, std::unique_ptr<RenderPass>> m_renderPasses;
    };
}

#endif // !_SGE_RENDERER_H_
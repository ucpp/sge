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
        class RenderContext* m_context = nullptr;

        std::unique_ptr<RenderPass> m_forwardPass;
        std::unique_ptr<RenderPass> m_geometryPass;
        std::unique_ptr<RenderPass> m_lightingPass;
        std::unique_ptr<RenderPass> m_tonemappingPass;
        std::unique_ptr<RenderPass> m_brightnesPass;
        std::unique_ptr<RenderPass> m_blurPass;
        std::unique_ptr<RenderPass> m_bloomCombinePass;
        std::unique_ptr<RenderPass> m_ssaoPass;
        std::unique_ptr<RenderPass> m_finalPass;
    };
}

#endif // !_SGE_RENDERER_H_

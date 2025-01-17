#ifndef _SGE_RENDERER_H_
#define _SGE_RENDERER_H_

#include "pch.h"

#include "sge_non_copyable.h"
#include "sge_render_pass.h"
#include "sge_scene.h"
#include "sge_editor.h"

namespace SGE
{
    class Renderer : public NonCopyable
    {
    public:
        void Initialize(class RenderContext* context);
        void Render(Scene* scene, Editor* editor);
        void Shutdown();

    private:
        class RenderContext* m_context = nullptr;

        std::unique_ptr<RenderPass> m_forwardPass;
        std::unique_ptr<RenderPass> m_geometryPass;
        std::unique_ptr<RenderPass> m_lightingPass;
    };
}

#endif // !_SGE_RENDERER_H_

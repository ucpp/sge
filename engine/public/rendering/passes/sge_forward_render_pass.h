#ifndef _SGE_FORWARD_RENDER_PASS_H_
#define _SGE_FORWARD_RENDER_PASS_H_

#include "pch.h"
#include "sge_render_pass.h"

namespace SGE
{
    class ForwardRenderPass : public RenderPass
    {
    public:
        void Initialize(class RenderContext* context) override;
        void Render(class Scene* scene) override;
        void Shutdown() override;
    };
}

#endif // !_SGE_FORWARD_RENDER_PASS_H_
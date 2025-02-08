#ifndef _SGE_TONEMAPPING_RENDER_PASS_H_
#define _SGE_TONEMAPPING_RENDER_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class ToneMappingRenderPass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_TONEMAPPING_RENDER_PASS_H_
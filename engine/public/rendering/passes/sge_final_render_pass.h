#ifndef _SGE_FINAL_RENDER_PASS_H_
#define _SGE_FINAL_RENDER_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class FinalRenderPass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_FINAL_RENDER_PASS_H_
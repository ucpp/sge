#ifndef _SGE_FXAA_RENDER_PASS_H_
#define _SGE_FXAA_RENDER_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class FXAARenderPass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_FXAA_RENDER_PASS_H_
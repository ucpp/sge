#ifndef _SGE_SSAO_RENDER_PASS_H_
#define _SGE_SSAO_RENDER_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class SSAORenderPass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_BRIGHTNESS_EXTRACTION_PASS_H_
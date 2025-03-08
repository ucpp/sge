#ifndef _SGE_REFLECTION_CUBEMAP_PASS_H_
#define _SGE_REFLECTION_CUBEMAP_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class ReflectionCubemapPass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene, const std::vector<std::string>& input, const std::vector<std::string>& output) override;
        void OnDraw(class Scene* scene) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_REFLECTION_CUBEMAP_PASS_H_
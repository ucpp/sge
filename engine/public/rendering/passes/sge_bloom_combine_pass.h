#ifndef _SGE_BLOOM_COMBINE_PASS_H_
#define _SGE_BLOOM_COMBINE_PASS_H_

#include "pch.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class BloomCombinePass : public RenderPass
    {
    protected:
        void OnRender(class Scene* scene) override;
        PipelineConfig GetPipelineConfig() const override;
    };
}

#endif // !_SGE_BLOOM_COMBINE_PASS_H_
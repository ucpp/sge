#ifndef _SGE_RENDER_PASS_H_
#define _SGE_RENDER_PASS_H_

#include "pch.h"
#include "sge_pipeline_state.h"

namespace SGE
{
    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        virtual void Initialize(class RenderContext* context) = 0;
        virtual void Render(class Scene* scene) = 0;
        virtual void Shutdown() = 0;

        PipelineState* GetPSO() const { return m_pipelineState.get(); }

    protected:
        class RenderContext* m_context = nullptr;
        std::unique_ptr<PipelineState> m_pipelineState;
    };
}

#endif // !_SGE_RENDER_PASS_H_
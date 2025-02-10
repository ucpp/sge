#ifndef _SGE_RENDER_PASS_FACTORY_H_
#define _SGE_RENDER_PASS_FACTORY_H_

#include "pch.h"
#include "core/sge_singleton.h"
#include "rendering/passes/sge_render_pass.h"

namespace SGE
{
    class RenderPassFactory : public Singleton<RenderPassFactory>
    {
        friend class Singleton<RenderPassFactory>;
    public:
        using Creator = std::function<std::unique_ptr<RenderPass>()>;

        void Register(const std::string& passName, Creator creator);
        std::unique_ptr<RenderPass> Create(const std::string& passName);
        
    private:
        std::unordered_map<std::string, Creator> creators;
    };

    void RegisterRenderPasses();
}

#endif // !_SGE_RENDER_PASS_FACTORY_H_
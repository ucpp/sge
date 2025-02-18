#include "rendering/passes/sge_render_pass_factory.h"

#include "rendering/passes/sge_render_pass.h"
#include "rendering/passes/sge_forward_render_pass.h"
#include "rendering/passes/sge_geometry_render_pass.h"
#include "rendering/passes/sge_lighting_render_pass.h"
#include "rendering/passes/sge_tonemapping_render_pass.h"
#include "rendering/passes/sge_brightness_extraction_pass.h"
#include "rendering/passes/sge_blur_pass.h"
#include "rendering/passes/sge_bloom_combine_pass.h"
#include "rendering/passes/sge_ssao_render_pass.h"
#include "rendering/passes/sge_final_render_pass.h"
#include "rendering/passes/sge_fxaa_render_pass.h"
#include "rendering/passes/sge_skybox_render_pass.h"
#include "rendering/passes/sge_shadow_map_pass.h"
#include "rendering/passes/sge_depth_pre_pass.h"

namespace SGE
{   
    void RenderPassFactory::Register(const std::string& passName, Creator creator)
    {
        creators[passName] = std::move(creator);
    }

    std::unique_ptr<RenderPass> RenderPassFactory::Create(const std::string& passName)
    {
        auto it = creators.find(passName);
        if (it != creators.end())
        {
            return it->second();
        }

        throw std::runtime_error("Unknown render pass type: " + passName);
    }

    
    void RegisterRenderPasses()
    {
        auto& factory = RenderPassFactory::Get();

        factory.Register("forward", []() { return std::make_unique<ForwardRenderPass>(); });
        factory.Register("brightness_extraction", []() { return std::make_unique<BrightnessExtractionPass>(); });
        factory.Register("blur", []() { return std::make_unique<BlurPass>(); });
        factory.Register("bloom_combine", []() { return std::make_unique<BloomCombinePass>(); });
        factory.Register("tonemapping", []() { return std::make_unique<ToneMappingRenderPass>(); });
        factory.Register("final", []() { return std::make_unique<FinalRenderPass>(); });
        factory.Register("geometry", []() { return std::make_unique<GeometryRenderPass>(); });
        factory.Register("ssao", []() { return std::make_unique<SSAORenderPass>(); });
        factory.Register("lighting", []() { return std::make_unique<LightingRenderPass>(); });
        factory.Register("fxaa", []() { return std::make_unique<FXAARenderPass>(); });
        factory.Register("skybox", []() { return std::make_unique<SkyboxRenderPass>(); });
        factory.Register("shadow_map", []() { return std::make_unique<ShadowMapRenderPass>(); });
        factory.Register("depth_prepass", []() { return std::make_unique<DepthPreRenderPass>(); });
    }
}
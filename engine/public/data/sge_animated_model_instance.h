#ifndef _SGE_ANIMATED_MODEL_INSTANCE_H_
#define _SGE_ANIMATED_MODEL_INSTANCE_H_

#include "data/sge_model_instance.h"
#include "data/sge_animation.h"
#include "core/sge_math.h"

namespace SGE
{
    class AnimatedModelInstance : public ModelInstance
    {
    public:
        void Initialize(AnimatedModelAsset* asset, class Device* device, class DescriptorHeap* descriptorHeap, uint32 instanceIndex);

        void SelectAnimationForLayer(const std::string& animationName, int layer);
        void PlayAnimationForLayer(int layer);
        void StopAnimationForLayer(int layer);
        void ResetAnimationTimeForLayer(int layer);
        void SetCurrentAnimationTimeForLayer(int layer, float time);
        float GetCurrentAnimationTimeForLayer(int layer) const;
        float GetCurrentAnimationDurationForLayer(int layer) const;
        float GetTicksPerSecondForLayer(int layer) const;
        std::string GetCurrentAnimationNameForLayer(int layer) const;
        void ResetToTPose();

        void FixedUpdate(float deltaTime, bool forceUpdate = false) override;

        const std::vector<Animation>& GetAnimations() const;
        Skeleton& GetSkeleton() const;

    protected:
        const std::vector<Mesh>& GetMeshes() const override;
        void OnUpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix) override;

    private:
        void UpdateBoneTransformsForLayer(int32 boneIndex, const float4x4& parentTransform, const Animation& currentAnimation, float animationTime, int layer);
        void BlendBoneTransforms();

        AnimatedModelAsset* m_animatedAsset = nullptr;
        std::unordered_map<int, LayerAnimation> m_layerAnimations;
        std::vector<float4x4> m_finalBoneTransforms;
        std::unordered_map<int, std::vector<float4x4>> m_layerBoneTransforms; 
    };
}

#endif // !_SGE_ANIMATED_MODEL_INSTANCE_H_
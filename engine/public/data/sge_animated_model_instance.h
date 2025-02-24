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

        void SelectAnimation(const std::string& animationName);
        void PlayAnimation();
        void StopAnimation();
        void FixedUpdate(float deltaTime);

        void ResetAnimationTime();
        void SetCurrentAnimationTime(float time);
        float GetCurrentAnimationTime() const;
        float GetCurrentAnimationDuration() const;
        const std::vector<Animation>& GetAnimations() const;

    protected:
        const std::vector<Mesh>& GetMeshes() const override;
        void OnUpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix) override;
    
    private:
        AnimatedModelAsset* m_animatedAsset = nullptr;
        bool m_isPlaying = false;
        float m_currentAnimationTime = 0.0f;
        std::string m_currentAnimationName;
        std::vector<float4x4> m_boneTransforms;
    };
}

#endif // !_SGE_ANIMATED_MODEL_INSTANCE_H_
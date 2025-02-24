#include "data/sge_animated_model_instance.h"

namespace SGE
{
    float3 InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time)
    {
        if (keys.empty()) return float3::Zero;
        if (keys.size() == 1) return keys[0].position;

        for (size_t i = 0; i < keys.size() - 1; ++i)
        {
            if (time >= keys[i].time && time <= keys[i + 1].time)
            {
                float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);
                return lerp(keys[i].position, keys[i + 1].position, t);
            }
        }

        return keys.back().position;
    }

    float4 InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time)
    {
        if (keys.empty()) return float4::Identity;
        if (keys.size() == 1) return keys[0].rotation;

        for (size_t i = 0; i < keys.size() - 1; ++i)
        {
            if (time >= keys[i].time && time <= keys[i + 1].time)
            {
                float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);
                return slerp(keys[i].rotation, keys[i + 1].rotation, t);
            }
        }

        return keys.back().rotation;
    }

    float3 InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time)
    {
        if (keys.empty()) return float3::One;
        if (keys.size() == 1) return keys[0].scale;

        for (size_t i = 0; i < keys.size() - 1; ++i)
        {
            if (time >= keys[i].time && time <= keys[i + 1].time)
            {
                float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);
                return lerp(keys[i].scale, keys[i + 1].scale, t);
            }
        }

        return keys.back().scale;
    }

    void AnimatedModelInstance::Initialize(AnimatedModelAsset* asset, Device* device, DescriptorHeap* descriptorHeap, uint32 instanceIndex)
    {
        m_animatedAsset = asset;
        ModelInstance::Initialize(asset, device, descriptorHeap, instanceIndex);

        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
        m_boneTransforms.resize(skeleton.GetBoneCount(), float4x4::Identity);
    }

    void AnimatedModelInstance::SelectAnimation(const std::string& animationName)
    {
        const auto& animations = m_animatedAsset->GetAnimations();
        auto it = std::find_if(animations.begin(), animations.end(), [&animationName](const Animation& anim) { return anim.name == animationName; });

        if (it != animations.end())
        {
            m_currentAnimationName = animationName;
            m_currentAnimationTime = 0.0f;
        }
        else
        {
            throw std::runtime_error("Animation not found");
        }
    }

    void AnimatedModelInstance::PlayAnimation()
    {
        m_isPlaying = true;
    }

    void AnimatedModelInstance::StopAnimation()
    {
        m_isPlaying = false;
    }

    void AnimatedModelInstance::FixedUpdate(float deltaTime)
    {
        if (!m_isPlaying) return;

        const auto& animations = m_animatedAsset->GetAnimations();
        auto it = std::find_if(animations.begin(), animations.end(), [this](const Animation& anim) { return anim.name == m_currentAnimationName; });

        if (it == animations.end()) return;

        const Animation& currentAnimation = *it;
        m_currentAnimationTime += deltaTime * currentAnimation.ticksPerSecond;

        if (m_currentAnimationTime > currentAnimation.duration)
        {
            m_currentAnimationTime = fmod(m_currentAnimationTime, currentAnimation.duration);
        }

        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
        for (const auto& boneAnimation : currentAnimation.boneAnimations)
        {
            int32 boneIndex = skeleton.GetBoneIndex(boneAnimation.boneName);
            if (boneIndex == -1) continue;

            float3 position = InterpolatePosition(boneAnimation.positionKeys, m_currentAnimationTime);
            float4 rotation = InterpolateRotation(boneAnimation.rotationKeys, m_currentAnimationTime);
            float3 scale = InterpolateScale(boneAnimation.scaleKeys, m_currentAnimationTime);

            float4x4 translationMatrix = CreateTranslationMatrix(position);
            float4x4 rotationMatrix = CreateRotationMatrixFromQuaternion(rotation);
            float4x4 scaleMatrix = CreateScaleMatrix(scale);

            float4x4 transform = scaleMatrix * rotationMatrix * translationMatrix;

            m_boneTransforms[boneIndex] = transform * skeleton.GetBoneOffset(boneIndex);
        }
    }

    void AnimatedModelInstance::ResetAnimationTime()
    {
        m_currentAnimationTime = 0.0f;
    }

    void AnimatedModelInstance::SetCurrentAnimationTime(float time)
    {
        m_currentAnimationTime = time;
    }

    float AnimatedModelInstance::GetCurrentAnimationTime() const
    {
        return m_currentAnimationTime;
    }

    float AnimatedModelInstance::GetCurrentAnimationDuration() const
    {
        const auto& animations = m_animatedAsset->GetAnimations();
        auto it = std::find_if(animations.begin(), animations.end(), [this](const Animation& anim) {
            return anim.name == m_currentAnimationName;
        });

        if (it != animations.end())
        {
            return it->duration;
        }
        return 0.0f;
    }

    const std::vector<Animation>& AnimatedModelInstance::GetAnimations() const
    {
        return m_animatedAsset->GetAnimations();
    }

    void AnimatedModelInstance::OnUpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix)
    {
        m_transformData.model = GetWorldMatrix();
        m_transformData.view = viewMatrix;
        m_transformData.projection = projectionMatrix;
        m_transformData.isAnimated = true;
        
        size_t boneCount = m_boneTransforms.size();
        for (size_t i = 0; i < boneCount; ++i)
        {
            m_transformData.boneTransforms[i] = m_boneTransforms[i];
        }

        for (size_t i = m_boneTransforms.size(); i < 100; ++i)
        {
            m_transformData.boneTransforms[i] = float4x4::Identity;
        }

        int32 sizeofTransformBuffer = sizeof(TransformBuffer);

        m_transformBuffer.Update(&m_transformData, sizeof(TransformBuffer));
    }

    const std::vector<Mesh>& AnimatedModelInstance::GetMeshes() const
    {
        return m_animatedAsset->GetMeshes();
    }
}
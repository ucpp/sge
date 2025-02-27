#include "data/sge_animated_model_instance.h"

namespace SGE
{
    float4x4 ConvertToLeftHanded(const float4x4& matrix)
    {
        float4x4 result = matrix;

        result.m30 = -result.m30;
        result.m31 = -result.m31;
        result.m32 = -result.m32;

        result.m20 = -result.m20;
        result.m21 = -result.m21;
        result.m22 = -result.m22;
        result.m23 = -result.m23;

        return result;
    }

    float4 ConvertToLeftHanded(const float4& quat)
    {
        return float4(quat.x, quat.y, -quat.z, quat.w);
    }

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
            m_ticksPerSecond = it->ticksPerSecond;
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

    void AnimatedModelInstance::UpdateBoneTransforms(int32 boneIndex, const float4x4& parentTransform, const Animation& currentAnimation, float animationTime)
    {
        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
        const Bone& bone = skeleton.GetBone(boneIndex);

        auto it = currentAnimation.boneKeyframes.find(bone.name);
        if (it == currentAnimation.boneKeyframes.end())
        {
            m_boneTransforms[boneIndex] = parentTransform * bone.offsetMatrix;
            return;
        }

        const BoneKeyframes& boneKeyframes = it->second;

        float3 position = InterpolatePosition(boneKeyframes.positionKeys, animationTime);
        float4 rotation = InterpolateRotation(boneKeyframes.rotationKeys, animationTime);
        float3 scale = InterpolateScale(boneKeyframes.scaleKeys, animationTime);

        float4x4 translationMatrix = CreateTranslationMatrix(position);
        float4x4 rotationMatrix = CreateRotationMatrixFromQuaternion(rotation);
        float4x4 scaleMatrix = CreateScaleMatrix(scale);

        float4x4 localTransform = translationMatrix * rotationMatrix * scaleMatrix;
        float4x4 globalTransform = parentTransform * localTransform;
        m_boneTransforms[boneIndex] = globalTransform * bone.offsetMatrix;

        for (int32 childIndex : bone.children)
        {
            UpdateBoneTransforms(childIndex, globalTransform, currentAnimation, animationTime);
        }
    }
        
    void AnimatedModelInstance::FixedUpdate(float deltaTime, bool forceUpdate)
    {
        if (!m_isPlaying && !forceUpdate) return;

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
        for (int32 i = 0; i < skeleton.GetBoneCount(); ++i)
        {
            if (skeleton.GetBone(i).parentIndex == -1)
            {
                UpdateBoneTransforms(i, float4x4::Identity, currentAnimation, m_currentAnimationTime);
            }
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

    std::string AnimatedModelInstance::GetCurrentAnimationName() const
    {
        return m_currentAnimationName;
    }

    float AnimatedModelInstance::GetTicksPerSecond() const
    {
        return m_ticksPerSecond;
    }

    const std::vector<Animation>& AnimatedModelInstance::GetAnimations() const
    {
        return m_animatedAsset->GetAnimations();
    }

    Skeleton& AnimatedModelInstance::GetSkeleton() const
    {
        if (!m_animatedAsset)
        {
            throw std::runtime_error("AnimatedModelInstance has no associated AnimatedModelAsset.");
        }
        
        return m_animatedAsset->GetSkeleton();
    }

    void AnimatedModelInstance::OnUpdateTransform(const float4x4& viewMatrix, const float4x4& projectionMatrix)
    {
        m_transformData.model = GetWorldMatrix();
        m_transformData.view = viewMatrix;
        m_transformData.projection = projectionMatrix;
        m_transformData.isAnimated = true;
        
        size_t boneCount = min(100, m_boneTransforms.size());
        for (size_t i = 0; i < boneCount; ++i)
        {
            m_transformData.boneTransforms[i] = m_boneTransforms[i];
        }

        for (size_t i = boneCount; i < 100; ++i)
        {
            m_transformData.boneTransforms[i] = float4x4::Identity;
        }

        m_transformBuffer.Update(&m_transformData, sizeof(TransformBuffer));
    }

    const std::vector<Mesh>& AnimatedModelInstance::GetMeshes() const
    {
        return m_animatedAsset->GetMeshes();
    }
}
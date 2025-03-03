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
        size_t boneCount = skeleton.GetBoneCount();

        m_finalBoneTransforms.resize(boneCount, float4x4::Identity);
        for (int32 i = 0; i < 3; ++i)
        {
            m_layerBoneTransforms[i].resize(boneCount, float4x4::Identity);
        }
    }

    void AnimatedModelInstance::SelectAnimationForLayer(const std::string& animationName, int layer)
    {
        const auto& animations = m_animatedAsset->GetAnimations();
        auto it = std::find_if(animations.begin(), animations.end(), [&animationName](const Animation& anim) { return anim.name == animationName; });

        if (it != animations.end())
        {
            m_layerAnimations[layer] = { animationName, 0.0f, false, it->ticksPerSecond };
        }
        else
        {
            throw std::runtime_error("Animation not found");
        }
    }

    void AnimatedModelInstance::PlayAnimationForLayer(int layer)
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            m_layerAnimations[layer].isPlaying = true;
        }
    }

    void AnimatedModelInstance::StopAnimationForLayer(int layer)
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            m_layerAnimations[layer].isPlaying = false;
        }
    }

    void AnimatedModelInstance::ResetAnimationTimeForLayer(int layer)
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            m_layerAnimations[layer].currentTime = 0.0f;
        }
    }

    void AnimatedModelInstance::SetCurrentAnimationTimeForLayer(int layer, float time)
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            m_layerAnimations[layer].currentTime = time;
        }
    }

    float AnimatedModelInstance::GetCurrentAnimationTimeForLayer(int layer) const
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            return m_layerAnimations.at(layer).currentTime;
        }
        return 0.0f;
    }

    float AnimatedModelInstance::GetCurrentAnimationDurationForLayer(int layer) const
    {
        const auto& animations = m_animatedAsset->GetAnimations();
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            const std::string& animationName = m_layerAnimations.at(layer).animationName;
            auto it = std::find_if(animations.begin(), animations.end(), [&animationName](const Animation& anim) {
                return anim.name == animationName;
            });

            if (it != animations.end())
            {
                return it->duration;
            }
        }
        return 0.0f;
    }

    float AnimatedModelInstance::GetTicksPerSecondForLayer(int layer) const
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            return m_layerAnimations.at(layer).ticksPerSecond;
        }
        return 25.0f;
    }

    std::string AnimatedModelInstance::GetCurrentAnimationNameForLayer(int layer) const
    {
        if (m_layerAnimations.find(layer) != m_layerAnimations.end())
        {
            return m_layerAnimations.at(layer).animationName;
        }
        return "";
    }

    void AnimatedModelInstance::UpdateBoneTransformsForLayer(int32 boneIndex, const float4x4& parentTransform, const Animation& currentAnimation, float animationTime, int layer)
    {
        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
        const Bone& bone = skeleton.GetBone(boneIndex);

        auto it = currentAnimation.boneKeyframes.find(bone.name);
        if (it == currentAnimation.boneKeyframes.end())
        {
            m_layerBoneTransforms[layer][boneIndex] = parentTransform * bone.offsetMatrix;
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
        m_layerBoneTransforms[layer][boneIndex] = globalTransform * bone.offsetMatrix;

        for (int32 childIndex : bone.children)
        {
            UpdateBoneTransformsForLayer(childIndex, globalTransform, currentAnimation, animationTime, layer);
        }
    }

    void AnimatedModelInstance::BlendBoneTransforms()
    {
        std::fill(m_finalBoneTransforms.begin(), m_finalBoneTransforms.end(), float4x4::Identity);

        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();

        for (auto& layerAnim : m_layerAnimations)
        {
            int layer = layerAnim.first;
            const auto& boneTransforms = m_layerBoneTransforms[layer];
            for (size_t i = 0; i < boneTransforms.size(); ++i)
            {
                const Bone& bone = skeleton.GetBone(static_cast<uint32>(i));
                float boneWeight = bone.weights[layer];
                m_finalBoneTransforms[i] = m_finalBoneTransforms[i] * (1.0f - boneWeight) + boneTransforms[i] * boneWeight;
            }
        }
    }

    void AnimatedModelInstance::FixedUpdate(float deltaTime, bool forceUpdate)
    {
        for (auto& layerAnim : m_layerAnimations)
        {
            if (!layerAnim.second.isPlaying && !forceUpdate) continue;

            const auto& animations = m_animatedAsset->GetAnimations();
            auto it = std::find_if(animations.begin(), animations.end(), [&layerAnim](const Animation& anim) { return anim.name == layerAnim.second.animationName; });

            if (it == animations.end()) continue;

            const Animation& currentAnimation = *it;
            layerAnim.second.currentTime += deltaTime * currentAnimation.ticksPerSecond;

            if (layerAnim.second.currentTime > currentAnimation.duration)
            {
                layerAnim.second.currentTime = fmod(layerAnim.second.currentTime, currentAnimation.duration);
            }

            const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
            for (int32 i = 0; i < skeleton.GetBoneCount(); ++i)
            {
                if (skeleton.GetBone(i).parentIndex == -1)
                {
                    UpdateBoneTransformsForLayer(i, float4x4::Identity, currentAnimation, layerAnim.second.currentTime, layerAnim.first);
                }
            }
        }

        BlendBoneTransforms();
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
        
        size_t boneCount = min(100, m_finalBoneTransforms.size());
        for (size_t i = 0; i < boneCount; ++i)
        {
            m_transformData.boneTransforms[i] = m_finalBoneTransforms[i];
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

    void AnimatedModelInstance::ResetToTPose()
    {
        for (auto& layerAnim : m_layerAnimations)
        {
            layerAnim.second.isPlaying = false;
            layerAnim.second.currentTime = 0.0f;
        }

        const Skeleton& skeleton = m_animatedAsset->GetSkeleton();
        for (int32 i = 0; i < skeleton.GetBoneCount(); ++i)
        {
            const Bone& bone = skeleton.GetBone(i);
            m_finalBoneTransforms[i] = float4x4::Identity;

            for (int layer = 0; layer < 3; ++layer)
            {
                m_layerBoneTransforms[layer][i] = m_finalBoneTransforms[i];
            }
        }

        BlendBoneTransforms();
    }
}
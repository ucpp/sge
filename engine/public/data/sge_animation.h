#ifndef _SGE_ANIMATION_H_
#define _SGE_ANIMATION_H_

#include "pch.h"

namespace SGE
{
    struct PositionKeyframe
    {
        float time;
        float3 position;
    };

    struct RotationKeyframe
    {
        float time;
        float4 rotation;
    };

    struct ScaleKeyframe
    {
        float time;
        float3 scale;
    };

    struct BoneKeyframes
    {
        std::string boneName;
        std::vector<PositionKeyframe> positionKeys;
        std::vector<RotationKeyframe> rotationKeys;
        std::vector<ScaleKeyframe> scaleKeys;
    };

    class Animation
    {
    public:
        std::string name;
        float duration;
        float ticksPerSecond;
        std::unordered_map<std::string, BoneKeyframes> boneKeyframes;
    };
}

#endif // !_SGE_ANIMATION_H_
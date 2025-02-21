#include "data/sge_animated_model_instance.h"

namespace SGE
{
    void AnimatedModelInstance::Initialize(AnimatedModelAsset* asset, class Device* device, class DescriptorHeap* descriptorHeap, uint32 instanceIndex)
    {
        m_animatedAsset = asset;
        ModelInstance::Initialize(asset, device, descriptorHeap, instanceIndex);
    }
}
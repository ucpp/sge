#ifndef _SGE_ANIMATED_MODEL_INSTANCE_H_
#define _SGE_ANIMATED_MODEL_INSTANCE_H_

#include "data/sge_model_instance.h"
#include "data/sge_animation.h"

namespace SGE
{
    class AnimatedModelInstance : public ModelInstance
    {
    public:
        void Initialize(AnimatedModelAsset* asset, class Device* device, class DescriptorHeap* descriptorHeap, uint32 instanceIndex);

    private:
        AnimatedModelAsset* m_animatedAsset = nullptr;
    };
}

#endif // !_SGE_ANIMATED_MODEL_INSTANCE_H_
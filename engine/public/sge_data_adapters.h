#ifndef _SGE_DATA_ADAPTERS_H_
#define _SGE_DATA_ADAPTERS_H_

#include "pch.h"

namespace SGE
{
    void SyncData(class CameraData* data, class Camera* camera);
    void SyncData(class DirectionalLightData* data, struct DirectionalLight* camera);
}

#endif // !_SGE_DATA_ADAPTERS_H_
#ifndef _SGE_DATA_ADAPTERS_H_
#define _SGE_DATA_ADAPTERS_H_

#include "pch.h"

namespace SGE
{
    void SyncData(const class CameraData* data, class Camera* camera);
    void SyncData(const class DirectionalLightData* data, struct DirectionalLight* light);
    void SyncData(const class ModelData* data, class ModelInstance* model);
    void SyncData(const class PointLightData* data, struct PointLight* pointLight);
}

#endif // !_SGE_DATA_ADAPTERS_H_
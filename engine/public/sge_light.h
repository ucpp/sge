#ifndef _SGE_LIGHT_H_
#define _SGE_LIGHT_H_

#include "pch.h"

namespace SGE
{
    struct DirectionalLight
    {
        XMFLOAT3 direction;
        float padding;
        XMFLOAT3 color;
        float intensity;
    };

    struct PointLight
    {
        XMFLOAT3 position;
        float padding;
        XMFLOAT3 color;
        float intensity;
    };

    struct LightData
    {
        DirectionalLight directionalLight;
        PointLight pointLight;
        XMFLOAT3 cameraPosition;
        float padding;
    };
}

#endif // !_SGE_LIGHT_H_
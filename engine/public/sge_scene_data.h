#ifndef _SGE_LIGHT_H_
#define _SGE_LIGHT_H_

#include "pch.h"

namespace SGE
{
    struct DirectionalLight
    {
        XMFLOAT3 direction;
        float intensity;
        XMFLOAT3 color;
        float padding;
    };

    struct PointLight
    {
        XMFLOAT3 position;
        float intensity;
        XMFLOAT3 color;
        float padding;
    };

    struct SceneData
    {
        DirectionalLight directionalLight;
        PointLight pointLight;
        XMFLOAT3 cameraPosition;
        float fogStrength;
        XMFLOAT3 fogColor;
        float fogStart;
        float fogEnd;
        float fogDensity;
        float padding[2];
    };
}

#endif // !_SGE_LIGHT_H_
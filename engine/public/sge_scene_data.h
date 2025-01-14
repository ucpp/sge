#ifndef _SGE_LIGHT_H_
#define _SGE_LIGHT_H_

#include "pch.h"

namespace SGE
{
    struct DirectionalLight
    {
        Vector3 direction;
        float intensity;
        Vector3 color;
        float padding;
    };

    struct PointLight
    {
        Vector3 position;
        float intensity;
        Vector3 color;
        float padding;
    };

    struct SceneData
    {
        DirectionalLight directionalLight;
        PointLight pointLights[MAX_POINT_LIGHTS];
        Vector3 cameraPosition;
        float fogStrength;
        Vector3 fogColor;
        float fogStart;
        float fogEnd;
        float fogDensity;
        float zNear;
        float zFar;
        Matrix invViewProj;
    };
}

#endif // !_SGE_LIGHT_H_
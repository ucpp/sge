#ifndef _SGE_SCENE_DATA_H_
#define _SGE_SCENE_DATA_H_

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

    struct SpotLight
    {
        Vector3 position;
        float intensity;
        Vector3 direction;
        float innerConeCos;
        Vector3 color;
        float outerConeCos;
    };

    struct SceneData
    {
        DirectionalLight directionalLight;
        PointLight pointLights[MAX_POINT_LIGHTS];
        SpotLight spotLights[MAX_SPOT_LIGHTS];
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

#endif // !_SGE_SCENE_DATA_H_
#ifndef _SGE_APPLICATION_SETTINGS_H_
#define _SGE_APPLICATION_SETTINGS_H_

#include "pch.h"

namespace SGE
{
    struct WindowSettings
    {
        std::string title = "SGE";
        uint32 width = 640;
        uint32 height = 480;
        bool fullscreen = false;
    };

    struct RenderSettings
    {
        std::array<float, 4> backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA
        bool isMSAAEnabled = true;
        bool isFogEnabled = false;
        bool isDeferredRendering = false;
    };

    struct EditorSettings
    {
        bool isPressedQuit = false;
    };

    struct MaterialSettings
    {
        std::string id;
        std::string vertexShaderPath;
        std::string pixelShaderPath;
        std::string albedoTexturePath;
        std::string normalTexturePath;
        std::string specularTexturePath;
    };

    struct RenderableAssetSettings
    {
        std::string id;
        std::string meshPath;
        std::string materialId;
    };

    struct RenderableInstanceSettings
    {
        std::string name;
        std::string assetId;
        std::array<float, 3> position;
        std::array<float, 3> rotation;
        std::array<float, 3> scale;
    };

    struct CameraSettings
    {
        std::array<float, 3> startPosition;
        std::array<float, 3> startRotation;
        float moveSpeed;
        float sensitivity;
    };

    struct ProjectAssets
    {
        std::vector<MaterialSettings> materials;
        std::vector<RenderableAssetSettings> renderableAssets;
    };

    struct SceneSettings
    {
        std::string name;
        CameraSettings mainCamera;
        std::vector<RenderableInstanceSettings> renderableObjects;
    };

    struct ApplicationSettings
    {
        WindowSettings window;
        EditorSettings editor;
        RenderSettings render;
        // SceneSettings scene;
        // ProjectAssets assets;
    };
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

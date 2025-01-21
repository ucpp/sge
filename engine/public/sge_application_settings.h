#ifndef _SGE_APPLICATION_SETTINGS_H_
#define _SGE_APPLICATION_SETTINGS_H_

#include "pch.h"

namespace SGE
{
    struct WindowSettings
    {
        std::string title = "SGE";
        int32 selectedResolution = 0;
        bool fullscreen = false;
        std::vector<std::string> resolutions;

        const char** GetResolutions();
        void UpdateResolutionCStrings();


        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

    private:
        std::vector<const char*> resolutionCStrings;
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

    enum class AssetType
    {
        StaticMesh = 0,
        Material   = 1,
        Light      = 2
    };

    enum class ObjectType
    {
        Camera           = 0,
        Mesh             = 1,
        PointLight       = 2,
        DirectionalLight = 3
    };

    struct AssetBase
    {
        std::string name;
        AssetType type;
    };

    struct ProjectAssets
    {
        std::vector<AssetBase> assets;
    };

    struct SceneObject
    {
        std::string name;
        ObjectType type;
    };

    struct SceneSettings
    {
        std::vector<SceneObject> objects;
    };

    struct ApplicationSettings
    {
        WindowSettings window;
        EditorSettings editor;
        RenderSettings render;
        ProjectAssets project;
        SceneSettings scene;
    };
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

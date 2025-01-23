#ifndef _SGE_APPLICATION_SETTINGS_H_
#define _SGE_APPLICATION_SETTINGS_H_

#include "pch.h"
#include "sge_scene_settings.h"

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
        bool isEnable = true;
        bool isPressedQuit = false;
    };

    enum class AssetType
    {
        StaticMesh = 0,
        Material   = 1,
        Light      = 2
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

    struct ApplicationSettings
    {
        WindowSettings window;
        EditorSettings editor;
        RenderSettings render;
        ProjectAssets project;
        SceneSettings scene;
    };

    const std::string DEFAULT_SETTINGS_PATH = "resources/configs/application_settings.json";
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

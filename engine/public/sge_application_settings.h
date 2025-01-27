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
        uint32 targetFPS = 30;

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
        Model = 0,
        Material   = 1,
        Light      = 2
    };

    class AssetBase
    {
    public:
        virtual ~AssetBase() = default;
        virtual void ToJson(nlohmann::json& j) const;
        virtual void FromJson(const nlohmann::json& j);
    
    public:
        std::string name;
        AssetType type;
    };

    class ModelAssetSettings : public AssetBase
    {
    public:
        ModelAssetSettings();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;
    
    public:
        std::string path;
    };

    class MaterialAssetSettings : public AssetBase
    {
    public:
        MaterialAssetSettings();
        void ToJson(nlohmann::json& j) const override;
        void FromJson(const nlohmann::json& j) override;

    public:
        std::string albedoTexturePath;
        std::string metallicTexturePath;
        std::string normalTexturePath;
        std::string roughnessTexturePath;
    };

    class LightAssetSettings : public AssetBase 
    {
    public:
        LightAssetSettings();
    }; 

    class ProjectAssets
    {
    public:
        const MaterialAssetSettings& GetMaterial(const std::string& id) const;
        const ModelAssetSettings& GetModel(const std::string& id) const;

    public:
        std::unordered_map<std::string, std::unique_ptr<AssetBase>> assets;
    };

    struct ApplicationSettings
    {
        WindowSettings window;
        EditorSettings editor;
        RenderSettings render;
        ProjectAssets project;
        SceneSettings scene;
    };

    std::unique_ptr<AssetBase> CreateObject(AssetType type);

    const std::string DEFAULT_SETTINGS_PATH = "resources/configs/application_settings.json";
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

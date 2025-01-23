#include "sge_config.h"

namespace SGE
{
    void to_json(nlohmann::json& j, const WindowSettings& settings)
    {
        j = nlohmann::json{
            { "title", settings.title },
            { "selected_resolution", settings.selectedResolution },
            { "fullscreen", settings.fullscreen },
            { "resolutions", settings.resolutions }
        };
    }

    void to_json(nlohmann::json& j, const RenderSettings& settings)
    {
        j = nlohmann::json{
            { "background_color", settings.backgroundColor },
            { "is_msaa_enabled", settings.isMSAAEnabled },
            { "is_fog_enabled", settings.isFogEnabled },
            { "is_deferred_rendering", settings.isDeferredRendering }
        };
    }

    void to_json(nlohmann::json& j, const ApplicationSettings& settings)
    {
        j = nlohmann::json{
            { "window", settings.window },
            { "render", settings.render },
            { "project", settings.project },
            { "scene", settings.scene },
            { "editor", settings.editor }
        };
    }

    void to_json(nlohmann::json& j, const EditorSettings& settings)
    {
        j = nlohmann::json
        {
            {"assets", settings.isEnable}
        };
    }

    void from_json(const nlohmann::json& j, WindowSettings& settings)
    {
        j.at("title").get_to(settings.title);
        j.at("selected_resolution").get_to(settings.selectedResolution);
        j.at("fullscreen").get_to(settings.fullscreen);
        j.at("resolutions").get_to(settings.resolutions);

        settings.UpdateResolutionCStrings();
    }

    void from_json(const nlohmann::json& j, RenderSettings& settings)
    {
        j.at("background_color").get_to(settings.backgroundColor);
        j.at("is_msaa_enabled").get_to(settings.isMSAAEnabled);
        j.at("is_fog_enabled").get_to(settings.isFogEnabled);
        j.at("is_deferred_rendering").get_to(settings.isDeferredRendering);
    }

    void from_json(const nlohmann::json& j, ApplicationSettings& settings)
    {
        j.at("window").get_to(settings.window);
        j.at("render").get_to(settings.render);
        j.at("project").get_to(settings.project);
        j.at("scene").get_to(settings.scene);
        j.at("editor").get_to(settings.editor);
    }

    void from_json(const nlohmann::json& j, EditorSettings& settings)
    {
        j.at("is_enable").get_to(settings.isEnable);
    }

    void to_json(nlohmann::json& j, const AssetBase& asset)
    {
        j = nlohmann::json{
            {"name", asset.name},
            {"type", static_cast<int>(asset.type)}
        };
    }

    void from_json(const nlohmann::json& j, AssetBase& asset)
    {
        j.at("name").get_to(asset.name);
        
        int32 typeInt;
        j.at("type").get_to(typeInt);
        asset.type = static_cast<AssetType>(typeInt);
    }

    void to_json(nlohmann::json& j, const ProjectAssets& projectAssets)
    {
        j = nlohmann::json{
            {"assets", projectAssets.assets}
        };
    }

    void from_json(const nlohmann::json& j, ProjectAssets& projectAssets)
    {
        j.at("assets").get_to(projectAssets.assets);
    }
}
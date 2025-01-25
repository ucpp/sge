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
            {"is_enable", settings.isEnable}
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

    void to_json(nlohmann::json& j, const ProjectAssets& settings)
    {
        nlohmann::json assetsJson = nlohmann::json::array();
        for (const auto& pair : settings.assets)
        {
            const auto& asset = pair.second;
            nlohmann::json assetJson;
            asset->ToJson(assetJson);
            assetsJson.push_back(assetJson);
        }
        j["assets"] = assetsJson;
    }

    void from_json(const nlohmann::json& j, ProjectAssets& settings)
    {
        settings.assets.clear();
        if (j.contains("assets"))
        {
            const auto& assetsJson = j.at("assets");
            for (const auto& assetJson : assetsJson)
            {
                int32_t typeInt;
                assetJson.at("type").get_to(typeInt);
                auto type = static_cast<AssetType>(typeInt);

                std::unique_ptr<AssetBase> asset;
                asset = CreateObject(type);

                asset->FromJson(assetJson);
                settings.assets[asset->name] = std::move(asset);
            }
        }
    }
}
#include "sge_config.h"

namespace SGE
{
    void to_json(nlohmann::json& j, const WindowSettings& settings)
    {
        j = nlohmann::json{
            { "title", settings.title },
            { "width", settings.width },
            { "height", settings.height },
            { "fullscreen", settings.fullscreen }
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
            { "render", settings.render }
        };
    }

    void from_json(const nlohmann::json& j, WindowSettings& settings)
    {
        j.at("title").get_to(settings.title);
        j.at("width").get_to(settings.width);
        j.at("height").get_to(settings.height);
        j.at("fullscreen").get_to(settings.fullscreen);
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
    }
}
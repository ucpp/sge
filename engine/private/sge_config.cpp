#include "sge_config.h"

namespace SGE
{
    void to_json(nlohmann::json& j, const ApplicationSettings& settings)
    {
        j = nlohmann::json{
            { "application_title", settings.title },
            { "screen_width", settings.width },
            { "screen_height", settings.height },
            { "fullscreen", settings.fullscreen },
            { "wireframe_mode", settings.wireframeMode }
        };
    }

    void from_json(const nlohmann::json& j, ApplicationSettings& settings)
    {
        j.at("application_title").get_to(settings.title);
        j.at("screen_width").get_to(settings.width);
        j.at("screen_height").get_to(settings.height);
        j.at("fullscreen").get_to(settings.fullscreen);
        j.at("wireframe_mode").get_to(settings.wireframeMode);
    }
}
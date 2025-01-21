#include "sge_application_settings.h"

namespace SGE
{
    const char **WindowSettings::GetResolutions()
    {
        return resolutionCStrings.data();
    }

    void WindowSettings::UpdateResolutionCStrings()
    {
        resolutionCStrings.clear();
        for (const auto& res : resolutions)
        {
            resolutionCStrings.push_back(res.c_str());
        }
    }
    
    uint32_t WindowSettings::GetWidth() const
    {
        if (selectedResolution >= resolutions.size())
        {
            throw std::out_of_range("Selected resolution index is out of range.");
        }

        const std::string& resolution = resolutions[selectedResolution];
        size_t xPos = resolution.find('x');
        if (xPos == std::string::npos)
        {
            throw std::invalid_argument("Invalid resolution format. Expected 'WidthxHeight'.");
        }

        return std::stoul(resolution.substr(0, xPos));
    }
    
    uint32_t WindowSettings::GetHeight() const
    {
        if (selectedResolution >= resolutions.size())
        {
            throw std::out_of_range("Selected resolution index is out of range.");
        }

        const std::string& resolution = resolutions[selectedResolution];
        size_t xPos = resolution.find('x');
        if (xPos == std::string::npos)
        {
            throw std::invalid_argument("Invalid resolution format. Expected 'WidthxHeight'.");
        }

        return std::stoul(resolution.substr(xPos + 1));
    }
}
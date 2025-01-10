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

    struct ApplicationSettings
    {
        WindowSettings window;
        EditorSettings editor;
        RenderSettings render;
    };
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

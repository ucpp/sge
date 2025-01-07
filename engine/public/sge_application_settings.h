#ifndef _SGE_APPLICATION_SETTINGS_H_
#define _SGE_APPLICATION_SETTINGS_H_

#include "pch.h"

namespace SGE
{
    struct ApplicationSettings
    {
        std::string title = "App";
        uint32 width = 640;
        uint32 height = 480;
        bool fullscreen = false;

        // editor
        bool isPressedQuit = false;
        bool isMSAAEnabled = true;
        bool isFogEnabled = false;
    };
}

#endif // !_SGE_APPLICATION_SETTINGS_H_

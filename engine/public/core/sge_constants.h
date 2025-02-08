#ifndef _SGE_CONSTANTS_H_
#define _SGE_CONSTANTS_H_

#include "core/sge_types.h"

namespace SGE
{
    constexpr bool USE_WARP_DEVICE = false;
    constexpr uint32 BUFFER_COUNT = 2;
    
    constexpr uint32 CBV_HEAP_CAPACITY = 256;
    constexpr uint32 SRV_HEAP_CAPACITY = 256;
    constexpr uint32 CBV_SRV_HEAP_CAPACITY = CBV_HEAP_CAPACITY + SRV_HEAP_CAPACITY;
    
    constexpr uint32 DSV_HEAP_CAPACITY = 1;
    constexpr uint32 RTV_HEAP_CAPACITY = BUFFER_COUNT * 2 + 10;

    constexpr uint32 GBUFFER_START_RTV_HEAP_INDEX = BUFFER_COUNT * 2;
    constexpr uint32 RTT_START_RTV_HEAP_INDEX = GBUFFER_START_RTV_HEAP_INDEX + 3;
    constexpr uint32 GBUFFER_START_SRV_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 10;
    constexpr uint32 RTT_START_SRV_HEAP_INDEX = GBUFFER_START_SRV_HEAP_INDEX + 3;
    constexpr uint32 DEPTH_BUFFER_START_SRV_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 2;
    constexpr uint32 TEXTURES_START_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - SRV_HEAP_CAPACITY;
    constexpr uint32 EDITOR_START_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 60;

    constexpr float CLEAR_COLOR[4] = { 0.0, 0.0, 0.0, 1.0 };

    constexpr uint32 MAX_POINT_LIGHTS = 20;
    constexpr uint32 MAX_SPOT_LIGHTS = 1;

#ifdef _DEBUG
    constexpr const char* SHADERS_DIRECTORY = "../../../engine/shaders";
#else
    constexpr const char* SHADERS_DIRECTORY = "shaders";
#endif

    constexpr const char* DEFAULT_SETTINGS_PATH = "resources/configs/application_settings.json";
}

#endif // !_SGE_CONSTANTS_H_
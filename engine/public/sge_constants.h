#ifndef _SGE_CONSTANTS_H_
#define _SGE_CONSTANTS_H_

#include "sge_types.h"

namespace SGE
{
    constexpr bool USE_WARP_DEVICE = false;
    constexpr uint32 BUFFER_COUNT = 2;
    
    constexpr uint32 CBV_HEAP_CAPACITY = 128;
    constexpr uint32 SRV_HEAP_CAPACITY = 128;
    constexpr uint32 CBV_SRV_HEAP_CAPACITY = CBV_HEAP_CAPACITY + SRV_HEAP_CAPACITY;
    
    constexpr uint32 DSV_HEAP_CAPACITY = 1;
    constexpr uint32 RTV_HEAP_CAPACITY = BUFFER_COUNT * 2 + 3;

    constexpr uint32 GBUFFER_START_RTV_HEAP_INDEX = BUFFER_COUNT * 2;
    constexpr uint32 GBUFFER_START_SRV_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 10;
    constexpr uint32 DEPTH_BUFFER_START_SRV_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 2;
    constexpr uint32 TEXTURES_START_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - SRV_HEAP_CAPACITY;
    constexpr uint32 EDITOR_START_HEAP_INDEX = CBV_SRV_HEAP_CAPACITY - 20;

    constexpr float CLEAR_COLOR[4] = { 0.0, 0.0, 0.0, 1.0 };

    constexpr uint32 MAX_POINT_LIGHTS = 6;
    constexpr uint32 MAX_SPOT_LIGHTS = 1;
}

#endif // !_SGE_CONSTANTS_H_
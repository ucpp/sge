#ifndef _SGE_SCOPED_EVENT_H_
#define _SGE_SCOPED_EVENT_H_

#define USE_PIX
#include "pch.h"
#include <d3d12.h>
#include <pix3.h>

namespace SGE
{
    class ScopedEvent final
    {
    public:
        ScopedEvent(ID3D12GraphicsCommandList* commandList, const std::string& eventName);
        ~ScopedEvent();
    };
}

#ifdef _DEBUG
    #define SCOPED_EVENT(commandList, eventName) SGE::ScopedEvent event(commandList, eventName)
    #define SCOPED_EVENT_GPU(commandList, eventName) PIXScopedEvent(commandList, PIX_COLOR_DEFAULT, eventName);
#else
    #define SCOPED_EVENT(commandList, eventName)
    #define SCOPED_EVENT_GPU(commandList, eventName)
#endif

#endif // !_SGE_SCOPED_EVENT_H_
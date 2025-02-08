#ifndef _SGE_SCOPED_EVENT_H_
#define _SGE_SCOPED_EVENT_H_

#include "pch.h"
#include <d3d12.h>
//#include <pix.h>

namespace SGE
{
    class ScopedEvent final
    {
    public:
        ScopedEvent(const std::string& eventName);
        ~ScopedEvent();
    };
}

#ifdef _DEBUG
    #define SCOPED_EVENT(eventName) SGE::ScopedEvent event(eventName)
#else
    #define SCOPED_EVENT(eventName)
#endif

#endif // !_SGE_SCOPED_EVENT_H_
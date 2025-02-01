#ifndef _SGE_SCOPED_EVENT_H_
#define _SGE_SCOPED_EVENT_H_

#include "pch.h"
#include <d3d12.h>

namespace SGE
{
    class ScopedEvent final
    {
    public:
        ScopedEvent(ID3D12GraphicsCommandList* commandList, const std::string& eventName);
        ~ScopedEvent();
    
    private:
        ID3D12GraphicsCommandList* m_commandList;
    };
}

#ifdef _DEBUG
    #define SCOPED_EVENT(commandList, eventName) SGE::ScopedEvent event(commandList, eventName)
#else
    #define SCOPED_EVENT(commandList, eventName)
#endif

#endif // !_SGE_SCOPED_EVENT_H_
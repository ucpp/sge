#include "core/sge_scoped_event.h"

#include "core/sge_helpers.h"

namespace SGE
{
    ScopedEvent::ScopedEvent(const std::string& eventName)
    {
        //PIXBeginEvent(0, eventName.c_str());
    }
    
    ScopedEvent::~ScopedEvent()
    {
        //PIXEndEvent();
    }
}
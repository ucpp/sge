#include "sge_scoped_event.h"

#include "sge_helpers.h"

namespace SGE
{
    ScopedEvent::ScopedEvent(ID3D12GraphicsCommandList* commandList, const std::string& eventName) : m_commandList(commandList)
    {
        Verify(m_commandList, "ScopedEvent::ScopedEvent: Command list is null!");
        if(m_commandList)
        {
            const std::wstring wstrEventName = StringToWideString(eventName);
            m_commandList->BeginEvent(0, wstrEventName.c_str(), static_cast<uint32>(wstrEventName.size() * sizeof(wchar_t)));
        }
    }
    
    ScopedEvent::~ScopedEvent()
    {
        if(m_commandList)
        {
            m_commandList->EndEvent();
        }
    }
}
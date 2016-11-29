#include "ControllerConnectionEvent.h"

namespace Farlor
{
    const EventType ControllerConnectionEvent::m_eventType("controller_connection_event");

    ControllerConnectionEvent::ControllerConnectionEvent(u32 controllerNum, u32 connected)
    {
        m_controllerNum = controllerNum;
        m_connected = connected;
    }

    EventType ControllerConnectionEvent::GetEventType()
    {
        return m_eventType;
    }
}

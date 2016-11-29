#include "ThumbstickEvent.h"

namespace Farlor
{
    const EventType ThumbstickEvent::m_eventType("thumbstick_event");

    ThumbstickEvent::ThumbstickEvent(float lx, float ly, float rx, float ry)
        : m_lx{lx}
        , m_ly{ly}
        , m_rx{rx}
        , m_ry{ry}
    {
    }

    EventType ThumbstickEvent::GetEventType()
    {
        return m_eventType;
    }
}

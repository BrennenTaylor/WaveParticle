#include "ButtonEvent.h"

namespace Farlor
{
    const EventType ButtonEvent::m_eventType("button_event");

    ButtonEvent::ButtonEvent(int controllerNum, ButtonType buttonType, u32 pressed)
        : m_controllerNum(controllerNum)
        , m_buttonType(buttonType)
        , m_pressed(pressed)
    {
    }

    EventType ButtonEvent::GetEventType()
    {
        return m_eventType;
    }
}

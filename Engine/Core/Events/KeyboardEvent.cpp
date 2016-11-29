#include "KeyboardEvent.h"

namespace Farlor
{
    const EventType KeyboardEvent::m_eventType("keyboard_input_event");

    KeyboardEvent::KeyboardEvent(Keys keyPressed, u32 pressed)
    {
        m_keyPressed = keyPressed;
        m_pressed = pressed;
    }

    EventType KeyboardEvent::GetEventType()
    {
        return m_eventType;
    }
}

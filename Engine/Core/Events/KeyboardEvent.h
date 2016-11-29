#pragma once

#include "EventData.h"

#include "../../Input/Keys.h"

namespace Farlor
{
    struct KeyboardEvent : public EventData
    {
        KeyboardEvent(Keys keyPressed, u32 pressed);

        EventType GetEventType() override;

        static const EventType m_eventType;
        Keys m_keyPressed;
        u32 m_pressed;
    };

}

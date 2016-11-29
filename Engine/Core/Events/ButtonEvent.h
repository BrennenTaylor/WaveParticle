#pragma once

#include "EventData.h"

#include "../../Input/Buttons.h"

namespace Farlor
{
    struct ButtonEvent : public EventData
    {
        ButtonEvent(int controllerNum, ButtonType buttonType, u32 pressed);

        EventType GetEventType() override;

        static const EventType m_eventType;

        int m_controllerNum;
        ButtonType m_buttonType;
        u32 m_pressed;
    };
}

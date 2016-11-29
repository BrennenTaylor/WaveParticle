#pragma once

#include "EventData.h"

namespace Farlor
{
    struct ControllerConnectionEvent : public EventData
    {
        ControllerConnectionEvent(u32 controllerNum, u32 connected);

        EventType GetEventType() override;

        static const EventType m_eventType;
        u32 m_controllerNum;
        u32 m_connected;
    };

}

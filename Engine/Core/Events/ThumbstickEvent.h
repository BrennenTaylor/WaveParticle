#pragma once

#include "EventData.h"

namespace Farlor
{
    struct ThumbstickEvent : public EventData
    {
        ThumbstickEvent(float lx, float ly, float rx, float ry);

        EventType GetEventType() override;

        static const EventType m_eventType;

        float m_lx;
        float m_ly;
        float m_rx;
        float m_ry;
    };
}

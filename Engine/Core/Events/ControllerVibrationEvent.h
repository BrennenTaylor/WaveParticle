#pragma once

#include "EventData.h"

namespace Farlor
{
    struct ControllerVibrationEvent : public EventData
    {
        enum class MotorControl
        {
            Left,
            Right
        };

        ControllerVibrationEvent(int controllerNum, float leftLevel, float rightLevel);

        EventType GetEventType() override;

        static const EventType m_eventType;

        int m_controllerNum;
        float m_leftVibration;
        float m_rightVibration;
    };
}

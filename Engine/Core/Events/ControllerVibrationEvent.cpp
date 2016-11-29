#include "ControllerVibrationEvent.h"

namespace Farlor
{
    const EventType ControllerVibrationEvent::m_eventType("controller_vibration_event");

    ControllerVibrationEvent::ControllerVibrationEvent(int controllerNum, float leftVibration, float rightVibration)
        : m_controllerNum(controllerNum)
        , m_leftVibration(leftVibration)
        , m_rightVibration(rightVibration)
    {
    }

    EventType ControllerVibrationEvent::GetEventType()
    {
        return m_eventType;
    }
}

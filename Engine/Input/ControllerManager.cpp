#include "ControllerManager.h"

#include "../Core/Events/ButtonEvent.h"
#include "../Core/Events/ControllerConnectionEvent.h"
#include "../Core/Events/ControllerVibrationEvent.h"
#include "../Core/Events/EventManager.h"
#include "../Core/Events/ThumbstickEvent.h"

#include <iostream>
using namespace std;

namespace Farlor
{
    extern EventManager g_EventManager;

    ControllerManager::ControllerManager()
        : m_connectionStatus {false, false, false, false}
        , m_initialized {false, false, false, false}
    {
    }

    ControllerManager::~ControllerManager()
    {
        // Turn off controller vibration
        for (u32 i = 0; i < XUSER_MAX_COUNT; i++)
        {
            XINPUT_VIBRATION vibration;
            vibration.wLeftMotorSpeed = 0;
            vibration.wRightMotorSpeed = 0;
            XInputSetState(i, &vibration);
        }
    }

    void ControllerManager::PollConnection()
    {
        DWORD result;
        for(u32 controllerNum = 0; controllerNum < XUSER_MAX_COUNT; controllerNum++)
        {
            if (!m_connectionStatus[controllerNum])
            {
                XINPUT_STATE state = {0};
                result = XInputGetState(controllerNum, &state);

                if (result == ERROR_SUCCESS)
                {
                    m_connectionStatus[controllerNum] = true;
                    EventDataPtr newEvent(new ControllerConnectionEvent(controllerNum, true));
                    g_EventManager.QueueEvent(newEvent);
                }
            }
        }
    }

    void ControllerManager::PollStates()
    {
        DWORD result;
        for(u32 controllerNum = 0; controllerNum < XUSER_MAX_COUNT; controllerNum++)
        {
            if (m_connectionStatus[controllerNum])
            {
                XINPUT_STATE state = {0};
                result = XInputGetState(controllerNum, &state);

                if(result != ERROR_SUCCESS)
                {
                    m_connectionStatus[controllerNum] = false;
                    EventDataPtr newEvent(new ControllerConnectionEvent(controllerNum, false));
                    g_EventManager.QueueEvent(newEvent);
                }

                if(!m_initialized[controllerNum])
                {
                    m_initialized[controllerNum] = true;
                    m_prevStates[controllerNum] = state;
                    return;
                }

                // 4 Buttons on right
                // Handle getting status stuff
                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_A))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_A, (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_X))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_X, (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_Y))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Y, (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_B))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_B, (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                // Start and Select
                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_START))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Start, (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_BACK))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Select, (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                // D-Pad
                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_DPAD_DOWN))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Pad_Down, (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_DPAD_LEFT))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Pad_Left, (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_DPAD_UP))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Pad_Up, (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                if (DifferingStates(controllerNum, state, XINPUT_GAMEPAD_DPAD_RIGHT))
                {
                    EventDataPtr buttonEvent(new ButtonEvent(controllerNum,
                        ButtonType::Button_Pad_Right, (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)));
                    g_EventManager.QueueEvent(buttonEvent);
                }

                // Left Stick
                float LX = state.Gamepad.sThumbLX;
                float LY = state.Gamepad.sThumbLY;

                float magnitudeLH = sqrt(LX*LX+LY*LY);

                float normalizedLX = LX / magnitudeLH;
                float normalizedLY = LY / magnitudeLH;

                float normalizedMagnitudeLH;

                if (magnitudeLH > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                {
                    if (magnitudeLH > 32767)
                        magnitudeLH = 32767;

                    magnitudeLH -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

                    // Optionally normailze the magnitudeLH with respect to expected range
                    // gets 0.0f - 1.0f
                    normalizedMagnitudeLH = magnitudeLH / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                }
                else
                {
                    magnitudeLH = 0.0f;
                    normalizedMagnitudeLH = 0.0f;
                    normalizedLX = 0.0f;
                    normalizedLY = 0.0f;
                }

                // Right Stick
                float RX = state.Gamepad.sThumbRX;
                float RY = state.Gamepad.sThumbRY;

                float magnitudeRH = sqrt(RX*RX+RY*RY);

                float normalizedRX = RX / magnitudeRH;
                float normalizedRY = RY / magnitudeRH;

                float normalizedMagnitudeRH;

                if (magnitudeRH > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                {
                    if (magnitudeRH > 32767)
                        magnitudeRH = 32767;

                    magnitudeRH -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

                    // Optionally normailze the magnitudeRH with respect to expected range
                    // gets 0.0f - 1.0f
                    normalizedMagnitudeRH = magnitudeRH / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

                    // cout << "NormalizedRX :" << normalizedRX << endl;
                    // cout << "NormalizedRY :" << normalizedRY << endl;
                }
                else
                {
                    magnitudeRH = 0.0f;
                    normalizedMagnitudeRH = 0.0f;
                    normalizedRX = 0.0f;
                    normalizedRY = 0.0f;
                }

                EventDataPtr thumbstickEvent(new ThumbstickEvent(normalizedLX, normalizedLY, normalizedRX, normalizedRY));
                g_EventManager.QueueEvent(thumbstickEvent);

                m_prevStates[controllerNum] = state;
            }
        }
    }

    bool ControllerManager::HandleEvent(EventDataPtr pEvent)
    {
        if (pEvent->GetEventType().m_id == EventType::HashString("controller_vibration_event"))
        {
            ControllerVibrationEvent* vibrationEvent = dynamic_cast<ControllerVibrationEvent*>(pEvent.get());
            if (vibrationEvent == nullptr)
            {
                return false;
            }

            // Check that the passed controller number is in range
            ASSERT(vibrationEvent->m_controllerNum < 4);

            float left = vibrationEvent->m_leftVibration * 65535;
            float right = vibrationEvent->m_rightVibration * 65535;

            if (left > 65535.0f)
            {
                left = 65535.0f;
            }
            if (right > 65535.0f)
            {
                right = 65535.0f;
            }
            if (left < 0.0f)
            {
                left = 0.0f;
            }
            if (right < 0.0f)
            {
                left = 0.0f;
            }

            XINPUT_VIBRATION vibration;
            vibration.wLeftMotorSpeed = (int)left;
            vibration.wRightMotorSpeed = (int)right;
            XInputSetState(vibrationEvent->m_controllerNum, &vibration);
        }

        return true;
    }


    bool ControllerManager::DifferingStates(int controllerNum, XINPUT_STATE state, DWORD button)
    {
        if((m_prevStates[controllerNum].Gamepad.wButtons & button) !=
            (state.Gamepad.wButtons & button))
        {
            return true;
        }

        return false;
    }
}

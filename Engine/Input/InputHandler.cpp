#include "InputHandler.h"

#include "../Core/Events/EventManager.h"

#include "../Core/Events/ButtonEvent.h"
#include "../Core/Events/KeyboardEvent.h"
#include "../Core/Events/ControllerConnectionEvent.h"
#include "../Core/Events/ControllerVibrationEvent.h"
#include "../Core/Events/ThumbstickEvent.h"

#include "../Core/Timer.h"

#include "../NewRenderer/Camera.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    const float speed = 100;
    extern EventManager g_EventManager;
    extern Timer g_TimerGame;
    extern Camera g_MainCamera;


    InputHandler::InputHandler()
    {
        m_lx = 0.0f;
        m_ly = 0.0f;
        m_rx = 0.0f;
        m_ry = 0.0f;

        // Add all key states
    }

    bool InputHandler::HandleEvent(EventDataPtr event)
    {
        if(event->GetEventType().m_id == EventType::HashString("keyboard_input_event"))
        {
            // cout << "Handeling keyboard event" << endl;
            // std::shared_ptr<KeyboardEvent> keyEvent(dynamic_cast<KeyboardEvent*>(event.get()));
            KeyboardEvent* keyEvent = dynamic_cast<KeyboardEvent*>(event.get());

            if(keyEvent == nullptr)
                return false;

            switch(keyEvent->m_keyPressed)
            {
                case Keys::Escape:
                {
                        PostQuitMessage(0);
                } break;

                case Keys::A:
                {
                    m_keyStates[Keys::A] = keyEvent->m_pressed;
                } break;
                case Keys::D:
                {
                    m_keyStates[Keys::D] = keyEvent->m_pressed;
                } break;
                case Keys::W:
                {
                    m_keyStates[Keys::W] = keyEvent->m_pressed;
                } break;
                case Keys::S:
                {
                    m_keyStates[Keys::S] = keyEvent->m_pressed;
                } break;
                case Keys::Left_Shift:
                {
                    m_keyStates[Keys::Left_Shift] = keyEvent->m_pressed;
                } break;
                case Keys::Left_Ctrl:
                {
                    m_keyStates[Keys::Left_Ctrl] = keyEvent->m_pressed;
                } break;

                case Keys::P:
                {
                    m_keyStates[Keys::P] = keyEvent->m_pressed;

                    if (keyEvent->m_pressed)
                    {
                        g_TimerGame.Toggle();
                    }
                };
            }
        }

        if(event->GetEventType().m_id == EventType::HashString("controller_connection_event"))
        {
            ControllerConnectionEvent* connectionEvent = dynamic_cast<ControllerConnectionEvent*>(event.get());

            if(connectionEvent == nullptr)
                return false;

            // cout << "Controller " << connectionEvent->m_controllerNum << "\'s connection status: " << connectionEvent->m_connected << endl;
        }

        if (event->GetEventType().m_id == EventType::HashString("button_event"))
        {
            ButtonEvent* buttonEvent = dynamic_cast<ButtonEvent*>(event.get());

            if (buttonEvent == nullptr)
                return false;

            std::string button = "";
            std::string state = "";

            switch (buttonEvent->m_buttonType)
            {
            case ButtonType::Button_A:
            {
                EventDataPtr vibrationEvent(new ControllerVibrationEvent(buttonEvent->m_controllerNum, 0.0f, 0.0f));
                g_EventManager.QueueEvent(vibrationEvent);
            }
            break;

            case ButtonType::Button_X:
            {
                EventDataPtr vibrationEvent(new ControllerVibrationEvent(buttonEvent->m_controllerNum, 1.0f, 0.0f));
                g_EventManager.QueueEvent(vibrationEvent);
            }
            break;

            case ButtonType::Button_Y:
            {
                EventDataPtr vibrationEvent(new ControllerVibrationEvent(buttonEvent->m_controllerNum, 1.0f, 1.0f));
                g_EventManager.QueueEvent(vibrationEvent);
            }
            break;

            case ButtonType::Button_B:
            {
                EventDataPtr vibrationEvent(new ControllerVibrationEvent(buttonEvent->m_controllerNum, 0.0f, 1.0f));
                g_EventManager.QueueEvent(vibrationEvent);
            }
            break;

            case ButtonType::Button_Start:
            {
                button = "Start";
            }
            break;

            case ButtonType::Button_Select:
            {
                button = "Select";
                PostQuitMessage(0);
            }
            break;

            case ButtonType::Button_Pad_Down:
            {
                button = "Pad Down";
            }
            break;

            case ButtonType::Button_Pad_Left:
            {
                button = "Pad Left";
            }
            break;

            case ButtonType::Button_Pad_Up:
            {
                button = "Pad Up";
            }
            break;

            case ButtonType::Button_Pad_Right:
            {
                button = "Pad Right";
            }
            break;

            default:
            {
                button = "ERROR";
            }
            break;
            }

            if (buttonEvent->m_pressed)
            {
                state = "pressed";
            }
            else
            {
                state = "released";
            }

            // cout << "Controller " << buttonEvent->m_controllerNum << "\'s " << button << " button was " << state << endl;
        }

        if(event->GetEventType().m_id == EventType::HashString("thumbstick_event"))
        {
            ThumbstickEvent* thumbstickEvent = dynamic_cast<ThumbstickEvent*>(event.get());

            if(thumbstickEvent == nullptr)
                return false;

            m_lx = thumbstickEvent->m_lx;
            m_ly = thumbstickEvent->m_ly;
            m_rx = thumbstickEvent->m_rx;
            m_ry = thumbstickEvent->m_ry;
        }

        return true;
    }

    u32 InputHandler::KeyPressed(Keys key)
    {
        return m_keyStates[key];
    }

    void InputHandler::PollThumbstick(float& lx, float& ly, float& rx, float&ry)
    {
        lx = m_lx;
        ly = m_ly;
        rx = m_rx;
        ry = m_ry;
    }

}

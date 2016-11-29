#pragma once

#include "..\Core\Events\EventListener.h"

#include "Keys.h"

namespace Farlor
{
    class InputHandler : public IEventListener
    {
    public:
        InputHandler();

        bool HandleEvent(EventDataPtr event);

        u32 KeyPressed(Keys key);
        void PollThumbstick(float& lx, float& ly, float& rx, float&ry);


    private:
        std::map<Keys, u32> m_keyStates;
        float m_lx;
        float m_ly;
        float m_rx;
        float m_ry;
    };
}

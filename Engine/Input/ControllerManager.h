#pragma once

#include "../Farlor.h"

#include "../Core/Events/EventListener.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>

namespace Farlor
{
    class ControllerManager : public IEventListener
    {
    public:
        ControllerManager();
        ~ControllerManager();

        void PollConnection();
        void PollStates();

        virtual bool HandleEvent(EventDataPtr pEvent) override;

    private:
        bool DifferingStates(int controllerNum, XINPUT_STATE state, DWORD button);

    private:
        u32 m_connectionStatus[XUSER_MAX_COUNT];
        u32 m_initialized[XUSER_MAX_COUNT];
        XINPUT_STATE m_prevStates[XUSER_MAX_COUNT];
    };
};

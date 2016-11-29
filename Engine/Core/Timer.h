#pragma once

#include <Windows.h>

namespace Farlor
{
    class Timer
    {
    public:
        Timer();

        float DeltaTime(); // In seconds
        float TotalTime();

        void Reset();
        void Start();
        void Stop();
        void Tick();

        void Toggle();

    private:

        double m_secondsPerCount;
        double m_deltaTime;

        __int64 m_baseTime;
        __int64 m_pausedTime;
        __int64 m_stopTime;
        __int64 m_prevTime;
        __int64 m_currentTime;

        bool m_stopped;
    };
}

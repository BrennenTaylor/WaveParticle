#pragma once

#include <Windows.h>

namespace Farlor
{
    class ParticleSystemTimer
    {
    public:
        ParticleSystemTimer();
        bool Initialize();
        double GetCurrentTime();

    private:
        LARGE_INTEGER m_frequency;
    };
}

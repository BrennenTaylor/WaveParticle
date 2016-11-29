#include "ParticleSystemTimer.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    ParticleSystemTimer::ParticleSystemTimer()
    {
    }

    bool ParticleSystemTimer::Initialize()
    {
        if(!QueryPerformanceFrequency(&m_frequency))
            return false;

        return true;
    }

    double ParticleSystemTimer::GetCurrentTime()
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        return (double)currentTime.QuadPart / (double)m_frequency.QuadPart;
    }
}

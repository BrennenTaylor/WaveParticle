#include "FixedUpdate.h"

namespace Farlor
{
    FixedUpdate::FixedUpdate(float updateRate)
        : m_updateRate(updateRate)
        , m_accumulator(0.0)
    {
    }

    bool FixedUpdate::ShouldUpdate()
    {
        if (m_accumulator >= m_updateRate)
        {
            m_accumulator -= m_updateRate;
            return true;
        }
        return false;
    }

    void FixedUpdate::AccumulateTime(float frameTime)
    {
        m_accumulator += frameTime;
    }

    float FixedUpdate::GetUpdateRate()
    {
        return m_updateRate;
    }
}

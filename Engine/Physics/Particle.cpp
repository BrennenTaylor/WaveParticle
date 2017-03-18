#include "Particle.h"

namespace Farlor
{
    WaveParticle::WaveParticle(Vector3 birthPos, Vector3 direction, float birthTime, float size, bool isActive)
        : m_birthPosition(std::move(birthPos))
        , m_currentPosition(m_birthPosition)
        , m_direction(std::move(direction))
        , m_amplitude{1.0f}
        , m_dispersionAngle{TWO_PI/8.0f}
        , m_birthTime(birthTime)
        , m_particleSize(size)
        // , m_sampleAngle(0.523599f)
        // , m_sampleDistance(15.0f)
        , m_active(isActive)
        , m_timeMoved{0.0f}
    {
    }

    bool WaveParticle::ShouldKill()
    {
        if (m_amplitude <= 0.1f)
        {
            return true;
        }
        return false;
    }
}

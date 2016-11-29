#pragma once

#include "../Math/Math.h"

namespace Farlor
{
    static float PI = 3.141592f;
    static float TWO_PI = 2 * PI;

    struct WaveParticle
    {
        Vector3 m_birthPosition;
        Vector3 m_currentPosition;
        Vector3 m_direction;
        float m_amplitude;
        float m_dispersionAngle;
        float m_birthTime;
        float m_particleSize;
        float m_sampleAngle;
        float m_sampleDistance;
        bool m_active;

        WaveParticle(Vector3 birthPos, Vector3 direction, float birthTime, float size = 20.0f, bool isActive = false);

        bool ShouldKill();
    };
}

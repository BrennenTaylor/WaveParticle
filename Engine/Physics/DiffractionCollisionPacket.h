#pragma once

#include <FMath/FMath.h>

namespace Farlor
{
    struct DiffractionCollisionPacket
    {
        Vector2 m_endPoint;
        Vector2 m_obstacleDir;
        Vector2 m_particleDir;

        DiffractionCollisionPacket(Vector2 endPoint, Vector2 obstacleDir, Vector2 particleDir)
        {
            m_endPoint = endPoint;
            m_obstacleDir = obstacleDir;
            m_particleDir = particleDir;
        }
    };
}
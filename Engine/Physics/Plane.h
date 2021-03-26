#pragma once

#include <FMath/FMath.h>

namespace Farlor
{
    struct Plane
    {
        Vector3 m_normal;
        Vector3 m_point;
        float m_elasticity;
        float m_friction;

        Plane(float nx, float ny, float nz, float x, float y, float z,
            float elasticity = 0.5f, float friction = 0.3f)
        {
            m_normal = Vector3(nx, ny, nz);
            m_point = Vector3(x, y, z);
            m_elasticity = elasticity;
            m_friction = friction;
        }
    };
}

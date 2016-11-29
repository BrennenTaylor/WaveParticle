#pragma once

#include "Vector3f.h"

namespace Farlor
{
    class Plane
    {
    public:
        Plane(float a, float b, float c, float d);

        Vector3 m_normal;
        float m_offset;
    }
}
#pragma once

#include <FMath/FMath.h>

namespace Farlor
{
    struct LineSegment
    {
        Vector3 m_first;
        Vector3 m_second;

        LineSegment(float x1, float y1, float x2, float y2);
    };
}

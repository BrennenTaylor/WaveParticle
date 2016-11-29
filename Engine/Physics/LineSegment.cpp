#pragma once

#include "LineSegment.h"

namespace Farlor
{
    LineSegment::LineSegment(float x1, float y1, float x2, float y2)
    {
        m_first = Vector3(x1, y1, 0.0f);
        m_second = Vector3(x2, y2, 0.0f);
    }
}

#pragma once

#include "Vector3.h"
#include "FastVector.h"

namespace Farlor
{
    class Line
    {
    public:
        Line(const Vector3& direction, const Vector3& origin);

        Vector3 m_direction;
        Vector3 m_origin;
    };
}

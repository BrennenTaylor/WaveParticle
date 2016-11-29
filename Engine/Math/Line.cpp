#include "Line.h"

namespace Farlor
{
    Line::Line(const Vector3& direction, const Vector3& origin)
        : m_direction(direction)
        , m_origin(origin)
    {
    }
}

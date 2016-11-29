#pragma once

#include "../Math/Math.h"

namespace Farlor
{
    struct Sample
    {
        Vector3 m_oldPosition;
        Vector3 m_newPosition;
        bool m_isCollision;
    };
}

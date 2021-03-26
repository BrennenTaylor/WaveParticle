#pragma once

#include <FMath/FMath.h>

#include "../ECS/Entity.h"

namespace Farlor
{
    struct TransformComponent
    {
        Entity m_entity;
        Vector3 m_position;
        Vector3 m_rotation;
        Vector3 m_scale;
    };
}

#pragma once

#include "../ECS/Entity.h"

#include <FMath/FMath.h>


namespace Farlor
{
    struct PhysicsComponent
    {
        Entity m_entity;
        Vector3 m_acceleration;
        float m_mass;
        bool m_isAtRest;

        PhysicsComponent();
    };
}

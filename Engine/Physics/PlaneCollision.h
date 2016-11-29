#pragma once

#include "PhysicsComponent.h"
#include "Plane.h"

namespace Farlor
{
    struct PlaneCollision
    {
        Plane* m_pPlane;
        PhysicsComponent* m_pComponent;
        float m_timestep;

        PlaneCollision(Plane* pPlane, PhysicsComponent* pComponent, float timestep)
            : m_pPlane(pPlane)
            , m_pComponent(pComponent)
            , m_timestep(timestep)
        {
        }
    };
}

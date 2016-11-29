#pragma once

#include <vector>

#include "PhysicsComponent.h"
#include "Plane.h"
#include "PlaneCollision.h"

namespace Farlor
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem();

        void Update(float deltaTime);

        void AddComponent(PhysicsComponent component);

        void AddCollisionPlane(Plane&& plane);

        void Reset();

    private:
        void CalculateAcceleration();
        void ApplyIntegration(float deltaTime);
        bool CheckPlaneCollisions();
        void SetNewValues();
        void HandleCollision(PlaneCollision& PlaneCollision);

    private:
        std::vector<PhysicsComponent> m_components;
        std::vector<Plane> m_collisionPlanes;
        std::vector<PlaneCollision> m_collisions;
        Vector3 m_gravity;
    };
}

#include "PhysicsSystem.h"

#include "../ECS/TransformManager.h"

namespace Farlor
{
    extern TransformManager g_TransformManager;

    PhysicsSystem::PhysicsSystem()
        : m_components{}
        , m_collisionPlanes{}
        , m_collisions{}
        , m_gravity{0.0f, -9.81f, 0.0f}
    {
    }

void PhysicsSystem::Update(float deltaTime)
    {
        float timeStepRemaining = deltaTime;

        for (auto& component : m_components)
        {
            TransformComponent transformComponent = g_TransformManager.GetComponent(component.m_entity);

            if (transformComponent.m_position.y < -50.0f)
            {
                transformComponent.m_position.y = 50.0f;
            }
            else
            {
                transformComponent.m_position += Vector3(0.0f, -10.0f, 0.0f) * deltaTime;
            }

            g_TransformManager.SetComponent(transformComponent, component.m_entity);
        }

    }

    void PhysicsSystem::AddComponent(PhysicsComponent component)
    {
        m_components.push_back(component);
    }

    void PhysicsSystem::AddCollisionPlane(Plane&& plane)
    {
        m_collisionPlanes.push_back(plane);
    }

    void PhysicsSystem::Reset()
    {
        m_components.clear();
    }

    void PhysicsSystem::CalculateAcceleration()
    {
    }

    void PhysicsSystem::ApplyIntegration(float dt)
    {
    }

    bool PhysicsSystem::CheckPlaneCollisions()
    {
        return false;
    }

    void PhysicsSystem::SetNewValues()
    {
    }

    void PhysicsSystem::HandleCollision(PlaneCollision& planeCollision)
    {
    }
}

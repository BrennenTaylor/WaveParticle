#include "TransformManager.h"

namespace Farlor
{
    void TransformManager::AddComponent(TransformComponent component)
    {
        m_components.push_back(component);
    }

    TransformComponent TransformManager::GetComponent(Entity entity)
    {
        TransformComponent result;
        for (auto& component : m_components)
        {
            if (component.m_entity == entity)
            {
                result = component;
                break;
            }
        }

        return result;
    }

    void TransformManager::SetComponent(TransformComponent newComponent, Entity entity)
    {
        for (auto& component : m_components)
        {
            if (component.m_entity == entity)
            {
                component = newComponent;
                break;
            }
        }
    }
}

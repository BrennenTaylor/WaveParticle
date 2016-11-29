#include <algorithm>

#include "EntityManager.h"

namespace Farlor
{
    const u32 ECSManager::MAX_NUM_COMPONENTS = 64;

    ECSManager::ECSManager()
        : m_createId{0}
        , m_entities{}
        , m_freeEntities{}
        , m_transformComponents{}
    {
    }

    Entity ECSManager::CreateEntity()
    {
        if(m_freeEntities.size() > 0)
        {
            auto entity = m_freeEntities.front();
            m_freeEntities.pop();
            m_entities.push_back(entity);
            return entity;
        }

        m_entities.push_back(m_createId);
        return m_createId++;
    }

    void ECSManager::FreeEntity(Entity entity)
    {
        // Remove all components with that id

        // Then delete the entity and add to free list
        auto position = std::find(m_entities.begin(), m_entities.end(), entity);
        if (position != m_entities.end()) // == myVector.end() means the element was not found
        {
            m_entities.erase(position);
            m_freeEntities.push(entity);
        }
    }
}

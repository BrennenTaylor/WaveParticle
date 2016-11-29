#pragma once

#include <vector>
#include <map>
#include <queue>

#include "..\Farlor.h"

#include "Entity.h"
#include "Component.h"

#include "TransformComponent.h"

namespace Farlor
{
    class ECSManager
    {
    public:
        static const u32 MAX_NUM_COMPONENTS;

        ECSManager();

        virtual Entity CreateEntity();
        virtual void FreeEntity(Entity entity);

    private:
        u32 m_createId;

        std::vector<Entity> m_entities;
        std::queue<u32> m_freeEntities;

        std::vector<Entity> m_transformComponents;
    };
}

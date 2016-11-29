#pragma once

#include "TransformComponent.h"

#include "Entity.h"

#include <vector>

namespace Farlor
{
    class TransformManager
    {
    public:
        void AddComponent(TransformComponent component);

        TransformComponent GetComponent(Entity entity);

        void SetComponent(TransformComponent component, Entity entity);

    public:
        std::vector<TransformComponent> m_components;
    };
}

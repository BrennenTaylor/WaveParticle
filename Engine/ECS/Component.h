#pragma once

#include "..\Util\HashedString.h"
#include "Entity.h"

namespace Farlor
{
    class Component
    {
    public:
        Component(Entity entity);

    public:
        Entity m_entity;
    };
}

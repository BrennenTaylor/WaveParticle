#pragma once

#include "string"

#include "../ECS/Entity.h"

#include "../Util/HashedString.h"

namespace Farlor
{
    struct RenderingComponent
    {
        Entity m_entity;
        HashedString m_mesh;
    };
}

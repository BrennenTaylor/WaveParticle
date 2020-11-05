#pragma once

#include "../ECS/Entity.h"
#include "../Util/HashedString.h"

#include <string>

namespace Farlor
{
    struct RenderingComponent
    {
        Entity m_entity;
        HashedString m_mesh;
    };
}

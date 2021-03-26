#pragma once

#include "../ECS/Entity.h"
#include <FMath/FMath.h>

#include <string>

namespace Farlor
{
    // struct DirectionalLightComponent
    // {
    //     Entity m_entity;
    //     Vector3 m_direction;
    // };
    //
    // struct PointLightComponent
    // {
    //     Entity m_entity;
    //     Vector3 m_position;
    //     Vector3 m_color;
    //     float m_intensity;
    // };

    struct LightComponent
    {
        Entity m_entity;
        
        Vector3 m_color;
        Vector3 m_direction;
        Vector2 m_spotlightAngles;
        Vector4 m_lightRange;
    };
}

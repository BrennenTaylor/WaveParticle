#pragma once

#include <string>

#include "RenderResourceFormat.h"
#include "RenderResourceType.h"

namespace Farlor
{
    struct RenderResourceDesc
    {
        std::string m_name;
        RenderResourceType m_type;
        int m_widthScale;
        int m_heightScale;
        RenderResourceFormat m_format;
    };
}

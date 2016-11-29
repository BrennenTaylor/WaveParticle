#pragma once

#include "../Farlor.h"

#include <string>

namespace Farlor
{
    class ResourceHandle
    {
    public:
        static std::string s_ResourcePath;

        ResourceHandle(std::string& filename, u32 isBinary);

        u8* m_buffer;
        u32 m_bufferSize;
    };
}

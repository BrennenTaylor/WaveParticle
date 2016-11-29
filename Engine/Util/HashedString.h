#pragma once

#include "..\Farlor.h"

namespace Farlor
{
    class HashedString
    {
    public:
        HashedString();
        HashedString(std::string stringToHash);

        static u64 HashString(std::string stringToHash);

        bool operator<(const HashedString& other) const
        {
            return (m_id < other.m_id);
        }
        bool operator==(const HashedString& other) const
        {
            return (m_id == other.m_id);
        }

        // Store as void* so it is displayed as hex for debugger
        u64 m_id;
        std::string m_hashedString;
    };
}

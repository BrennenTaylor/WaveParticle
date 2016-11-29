#include "HashedString.h"

using namespace std;

namespace Farlor
{
    HashedString::HashedString()
        : m_id{HashString("")}
        , m_hashedString{""}
    {
    }

    HashedString::HashedString(string stringToHash)
        : m_id{HashString(stringToHash)}
        , m_hashedString{stringToHash}
    {
    }

    u64 HashedString::HashString(const std::string stringToHash)
    {
        return hash<std::string>()(stringToHash);
    }
}

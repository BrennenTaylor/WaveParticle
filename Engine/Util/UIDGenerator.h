#pragma once

#include <vector>

namespace Farlor
{
    class UIDGenerator
    {
    public:
        UIDGenerator();
        void ReleaseID(unsigned int id);
        unsigned int GetID();

    private:
        std::vector<unsigned int> m_releasedIDs;
        unsigned int m_currentCount;

    };
}

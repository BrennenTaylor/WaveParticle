#pragma once

#include <vector>

using namespace std;

namespace Farlor
{
    class UIDGenerator
    {
    public:
        UIDGenerator();
        void ReleaseID(unsigned int id);
        unsigned int GetID();

    private:
        vector<unsigned int> m_releasedIDs;
        unsigned int m_currentCount;

    };
}

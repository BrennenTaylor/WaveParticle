#include "UIDGenerator.h"

namespace Farlor
{
    UIDGenerator::UIDGenerator()
        : m_releasedIDs{}
        , m_currentCount{0}
    {
    }

    void UIDGenerator::ReleaseID(unsigned int id)
    {
        m_releasedIDs.push_back(id);
    }

    unsigned int UIDGenerator::GetID()
    {
        unsigned int id;

        if(m_releasedIDs.size() > 0)
        {
            id = m_releasedIDs.back();
            m_releasedIDs.pop_back();
            return id;
        }

        id = ++m_currentCount;

        return id;
    }
}

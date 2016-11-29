#include "BaseAllocator.h"

namespace Farlor {

Allocator::Allocator(size_t size, void* pStart)
{
    m_pStart = pStart;
    m_size = size;

    m_usedMemory = 0;
    m_numAllocations = 0;
}

Allocator::~Allocator()
{
    ASSERT(m_numAllocations == 0 && m_usedMemory == 0);
    m_pStart = nullptr;
    m_size = 0;
}

}
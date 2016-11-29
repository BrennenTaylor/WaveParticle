#include "LinearAllocator.h"

namespace Farlor {

LinearAllocator::LinearAllocator(size_t size, void* pStart)
    : Allocator(size, pStart)
    , m_pCurrentPosition(pStart)
{
    ASSERT(size > 0);    
}

LinearAllocator::~LinearAllocator()
{
    m_pCurrentPosition = nullptr;
}

void* LinearAllocator::Allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0);
    u8 adjustment = AlignForwardAdjustment(m_pCurrentPosition, alignment);
    if(m_usedMemory + adjustment + size > m_size)
    {
        return nullptr;
    }

    uptr pAlignedAddress = (uptr)m_pCurrentPosition + adjustment;
    m_pCurrentPosition = (void*)(pAlignedAddress + size);
    m_usedMemory += size + adjustment;
    m_numAllocations++;
    return (void*) pAlignedAddress;
}

void LinearAllocator::Deallocate(void* pMemory)
{
    ASSERT(false && "Use clear() instead");
}

void LinearAllocator::Clear()
{
    m_numAllocations = 0;
    m_usedMemory = 0;
    m_pCurrentPosition = m_pStart;
}

}
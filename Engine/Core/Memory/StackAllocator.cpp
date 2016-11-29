#include "StackAllocator.h"

namespace Farlor {

StackAllocator::StackAllocator(size_t size, void* pStart)
    : Allocator(size, pStart)
    , m_pCurrentPosition(pStart)
{
    ASSERT(size > 0);
    #if _DEBUG
    m_pPrevPosition = nullptr;
    #endif
}

StackAllocator::~StackAllocator()
{
    #if _DEBUG
    m_pPrevPosition = nullptr;
    #endif

    m_pCurrentPosition = nullptr;
}

void* StackAllocator::Allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0);

    u8 adjustment = AlignForwardAdjustmentWithHeader(m_pCurrentPosition, alignment, sizeof(StackAllocationHeader));
    if (m_usedMemory + adjustment + size > m_size)
        return nullptr;

    void* pAlignedAddress = (void*)((uptr)m_pCurrentPosition + adjustment);

    StackAllocationHeader* header = (StackAllocationHeader*)((uptr)pAlignedAddress - sizeof(StackAllocationHeader));
    header->adjustment = adjustment;
    #if _DEBUG
    header->pPrevAddress = m_pPrevPosition;
    m_pPrevPosition = pAlignedAddress;
    #endif

    m_pCurrentPosition = (void*)((uptr)pAlignedAddress + size);
    m_usedMemory += size + adjustment;
    m_numAllocations++;
    return pAlignedAddress;
}

void StackAllocator::Deallocate(void* pMemory)
{
    ASSERT(pMemory == m_pPrevPosition);

    StackAllocationHeader* header = (StackAllocationHeader*)((uptr)pMemory - sizeof(StackAllocationHeader));
    m_usedMemory -= (uptr)m_pCurrentPosition - (uptr)pMemory + header->adjustment;
    m_pCurrentPosition = (void*)((uptr)pMemory - header->adjustment);

    #if _DEBUG
    m_pPrevPosition = header->pPrevAddress;
    #endif

    m_numAllocations--;
}

}
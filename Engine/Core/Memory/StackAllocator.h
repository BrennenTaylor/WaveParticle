#pragma once

#include "BaseAllocator.h"

namespace Farlor {

class StackAllocator : public Allocator
{
    StackAllocator(size_t size, void* pStart);
    ~StackAllocator();

    virtual void* Allocate(size_t size, u8 alignment) override;
    virtual void Deallocate(void* pMemory) override;

private:
    StackAllocator(const StackAllocator&); // Prevent copies
    StackAllocator& operator=(const StackAllocator&);

    struct StackAllocationHeader
    {
        #if _DEBUG
        void* pPrevAddress;
        #endif
        u8 adjustment;
    };

    #if _DEBUG
    void* m_pPrevPosition;
    #endif

    void* m_pCurrentPosition;
};

}
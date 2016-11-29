#pragma once

#include "BaseAllocator.h"

namespace Farlor {

class LinearAllocator : public Allocator
{
public:
    LinearAllocator(size_t size, void* pStart);
    ~LinearAllocator();

    virtual void* Allocate(size_t size, u8 alignment = 4) override;
    virtual void Deallocate(void* pMemory) override;
    virtual void Clear();

private:
    LinearAllocator(const LinearAllocator&); // Prevent copies
    LinearAllocator& operator=(const LinearAllocator&);

    void* m_pCurrentPosition;
};

} // Farlor

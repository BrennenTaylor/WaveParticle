#pragma once

#include "..\..\Farlor.h"

namespace Farlor {

/*
To n-byte align a memory address x, mask off log2n(n) lsb of x
Masking bits returns first n-byte aligned address before x
So to find after, mask (x + (alignment - 1))
*/

inline void* AlignForward(void* pAddress, u8 alignment)
{
    return (void*)((reinterpret_cast<uptr>(pAddress)
        + static_cast<uptr>(alignment - 1))
        & static_cast<uptr>(~(alignment - 1)));
}

// How much alignment is needed?
inline u8 AlignForwardAdjustment(const void* pAddress, u8 alignment)
{
    u8 adjustment = alignment - (reinterpret_cast<uptr>(pAddress)
        & static_cast<uptr>(alignment - 1));
    
    if(adjustment == alignment)
        return 0;

    return adjustment;
}

// Store a header before each allocation, use adjustment
// space to reduce memory overhead
inline u8 AlignForwardAdjustmentWithHeader(const void* pAddress,
    u8 alignment, u8 headerSize)
{
    u8 adjustment = AlignForwardAdjustment(pAddress, alignment);
    u8 neededSpace = headerSize;
    
    if(adjustment < neededSpace)
    {
        neededSpace -= adjustment;
        adjustment += adjustment * (neededSpace / alignment);

        if(neededSpace % alignment > 0)
            adjustment += alignment;
    }

    return adjustment;
}


}
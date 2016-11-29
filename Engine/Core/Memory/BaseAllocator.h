#pragma once

#include "MemoryAlignmentUtils.h"

#include "../../Farlor.h"

/*

Allocator model:

Allocate 1GB of memory for Free List allocator
Create Proxy Allocator that redirects all allocations to the Free List allocator
Initialize resource manager by passing pointer to proxy allocator
Register the proxy allocator in the memory usage tracker
Allocate 16MB of memory using free list allocator and create a linear allocator
Use the linear allocator to make small temporary allocations needed for game logic and clear before the end of each frame
The resouirce manager will create a pool allocator for every resource package it loads

*/


namespace Farlor
{
    class Allocator
    {
    public:
        Allocator(size_t size, void* pStart);
        virtual ~Allocator();

        virtual void* Allocate(size_t size, u8 alignment = 4) = 0;
        virtual void Deallocate(void* pMemory) = 0;

    public:
        void* m_pStart;
        size_t m_size;
        size_t m_usedMemory;
        size_t m_numAllocations;
    };


    template <class T>
    T* AllocateNew(Allocator& allocator)
    {
        return new (allocator.Allocate(sizeof(T), __alignof(T))) T;
    }

    template <class T>
    T* AllocateNew(Allocator& allocator, const T& t)
    {
        return new (allocator.Allocate(sizeof(T), __alignof(T))) T(t);
    }

    template <class T>
    void DeallocateDelete(Allocator& allocator, T& object)
    {
        object.~T();
        allocator.Deallocate(&object);
    }

    template<class T>
    T* allocateArray(Allocator& allocator, size_t length)
    {
        ASSERT(length != 0);

        u8 headerSize = sizeof(size_t)/sizeof(T);

        if(sizeof(size_t)%sizeof(T) > 0)
            headerSize += 1;

        //Allocate extra space to store array length in the bytes before the array
        T* p = ( (T*) allocator.Allocate(sizeof(T)*(length + headerSize), __alignof(T)) ) + headerSize;

        *( ((size_t*)p) - 1 ) = length;

        for(size_t i = 0; i < length; i++)
            new (&p[i]) T;

        return p;
    }

    template<class T>
    void deallocateArray(Allocator& allocator, T* array)
    {
        ASSERT(array != nullptr);

        size_t length = *( ((size_t*)array) - 1 );

        for(size_t i = 0; i < length; i++)
            array[i].~T();

        //Calculate how much extra memory was allocated to store the length before the array
        u8 headerSize = sizeof(size_t)/sizeof(T);

        if(sizeof(size_t)%sizeof(T) > 0)
            headerSize += 1;

        allocator.Deallocate(array - headerSize);
    }
}

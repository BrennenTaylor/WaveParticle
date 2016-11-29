#pragma once

#include "..\..\Farlor.h"

#include "EventData.h"

namespace Farlor
{
    class IEventListener
    {
    public:
        // Return false when listener didnt handle event
        // If handled, return true
        virtual bool HandleEvent(EventDataPtr event) = 0;
    };

    typedef IEventListener* EventListenerPtr;
}
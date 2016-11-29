#pragma once

#include "..\..\Farlor.h"
#include <list>
#include <set>
#include <map>

#include "EventData.h"
#include "EventListener.h"

namespace Farlor
{
    class EventManager
    {
    public:
        enum constants {INFINANT = 0xffffffff};
        EventManager();

        // Add handler for specific event type
        bool AddListener(EventListenerPtr handler, EventType type);
        // Remove listener/type pairing
        bool DeleteListener(EventListenerPtr handler, EventType type);
        // Add event to the queue
        bool QueueEvent(EventDataPtr event);
        // Start firing events for amount of time
        bool Tick(u64 maxMillis = INFINANT);

    public:
        typedef std::list<EventListenerPtr> EventListenerPtrList;
        typedef std::map<EventType, EventListenerPtrList> EventListenerMap;
        typedef std::list<EventDataPtr> EventQueue;

        EventListenerMap m_registry;
        EventQueue m_eventQueues[2];

        u32 m_activeQueue;
    };
}
#include "EventManager.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    EventManager::EventManager()
    {
        m_activeQueue = 0;
    }

    bool EventManager::AddListener(EventListenerPtr handler,
        EventType type)
    {
        // Find or create entry
        EventListenerPtrList& eventListnerList = m_registry[type];
        for(auto it = eventListnerList.begin(); it != eventListnerList.end(); ++it)
        {
            if(handler == (*it))
            {
                cout << "Attempted to double register handler" << endl;
                return false;
            }
        }

        eventListnerList.push_back(handler);
        return true;
    }

    bool EventManager::DeleteListener(EventListenerPtr handler,
        EventType type)
    {
        auto typeList = m_registry.find(type);
        if(typeList != m_registry.end())
        {
            EventListenerPtrList& listenerList = typeList->second;
            for(auto it = listenerList.begin(); it != listenerList.end(); ++it)
            {
                if((*it) == handler)
                {
                    listenerList.erase(it);
                    return true;
                }
            }
        }

        return false;
    }

    bool EventManager::QueueEvent(EventDataPtr event)
    {
        if(!event)
            return false;

        auto found = m_registry.find(event->GetEventType());
        if(found != m_registry.end())
        {
            m_eventQueues[m_activeQueue].push_back(event);
            return true;
        }

        cout << "Not queued, no listeners handle: " << event->GetEventType().m_hashedString << endl;

        return false;
    }

    //  NOTE: Currently does not support maxMillis
    bool EventManager::Tick(u64 maxMillis)
    {
        if(maxMillis != INFINANT)
        {
            cout << "Event Handler Does not support maxMillis currently" << endl;
        }

        int queueToProcess = m_activeQueue;
        m_activeQueue = (m_activeQueue + 1) % 2; // NOTE: Make this a const
        m_eventQueues[m_activeQueue].clear();

        while(!m_eventQueues[queueToProcess].empty())
        {
            EventDataPtr event = m_eventQueues[queueToProcess].front();
            m_eventQueues[queueToProcess].pop_front();

            const EventType eventType = event->GetEventType();
            auto found = m_registry.find(eventType);
            if(found != m_registry.end())
            {
                const EventListenerPtrList& eventListeners = found->second;

                for(EventListenerPtr listener : eventListeners)
                {
                    if (listener)
                    {
                        listener->HandleEvent(event);
                    }
                }
            }
        }

        return true;
    }
}
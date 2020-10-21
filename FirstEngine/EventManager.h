#pragma once

#include <list>
#include <queue>
#include <memory>

#include "FastDelegate.h"

#include "IEventManager.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager {

    std::map<EventTypeId, std::list<EventListenerDelegate>> m_eventListeners;
    GameTimer m_EventTimer;

    // There are two event queues here so that delegate methods can safely queue up new events. You can imagine an infinite loop where
    // two events queue up each other. Without two queues, the program would hang in an infinite loopand never break out of the event VUpdate() function.
    std::list<IEventDataPtr> m_queues[EVENTMANAGER_NUM_QUEUES];
    int m_activeQueue;  // index of actively processing queue; events enque to the opposing queue

public:
    explicit EventManager(const char* pName, bool setAsGlobal);
    virtual ~EventManager(void);

    virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type);
    virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type);

    virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const;
    virtual bool VQueueEvent(const IEventDataPtr& pEvent);
    virtual bool VAbortEvent(const EventTypeId& inType, bool allOfType);

    virtual bool VUpdate(float maxSec = MAX_EVENTS_DURATION);
};
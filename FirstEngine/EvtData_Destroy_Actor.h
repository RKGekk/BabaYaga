#pragma once

#include <memory>

#include "IEventData.h"
#include "BaseEventData.h"

// Sent when actors are destroyed	
class EvtData_Destroy_Actor : public BaseEventData {
    unsigned int m_id;
    const std::string m_eventName;

public:
    static const EventTypeId sk_EventType = 0x77dd2b3a;

    explicit EvtData_Destroy_Actor(unsigned int id = 0) : m_eventName("EvtData_Destroy_Actor") {
        m_id = id;
    }

    virtual const EventTypeId& VGetEventType() const {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy(void) const {
        return IEventDataPtr(new EvtData_Destroy_Actor(m_id));
    }

    virtual void VSerialize(std::ostream& out) const {
        out << m_id;
    }

    virtual void VDeserialize(std::istream& in) {
        in >> m_id;
    }

    virtual const std::string& GetName() const {
        return m_eventName;
    }

    unsigned int GetId() const {
        return m_id;
    }
};
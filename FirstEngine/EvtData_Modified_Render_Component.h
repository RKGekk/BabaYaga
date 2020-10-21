#pragma once

#include "BaseEventData.h"

// This event is sent out when a render component is changed
class EvtData_Modified_Render_Component : public BaseEventData {
    unsigned int m_id;
    const std::string m_eventName;

public:
    static const EventTypeId sk_EventType = 0xe86c7c31;

    virtual const EventTypeId& VGetEventType() const {
        return sk_EventType;
    }

    EvtData_Modified_Render_Component() : m_eventName("EvtData_Modified_Render_Component") {
        m_id = 0;
    }

    EvtData_Modified_Render_Component(unsigned int id) : m_eventName("EvtData_Modified_Render_Component") {
        m_id = id;
    }

    virtual void VSerialize(std::ostream& out) const {
        out << m_id;
    }

    virtual void VDeserialize(std::istream& in) {
        in >> m_id;
    }

    virtual IEventDataPtr VCopy() const {
        return IEventDataPtr(new EvtData_Modified_Render_Component(m_id));
    }

    virtual const std::string& GetName() const {
        return m_eventName;
    }

    unsigned int GetActorId() const {
        return m_id;
    }
};
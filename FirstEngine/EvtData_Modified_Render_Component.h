#pragma once

#include <string>

#include "BaseEventData.h"
#include "Actor.h"

// This event is sent out when a render component is changed
class EvtData_Modified_Render_Component : public BaseEventData {
    ActorId m_id;

public:
    static const EventTypeId sk_EventType = 0xe86c7c31;
    static const std::string sk_EventName;

    EvtData_Modified_Render_Component();
    EvtData_Modified_Render_Component(ActorId id);

    virtual const EventTypeId& VGetEventType() const;
    virtual void VSerialize(std::ostream& out) const;
    virtual void VDeserialize(std::istream& in);
    virtual IEventDataPtr VCopy() const;
    virtual const std::string& GetName() const;
    ActorId GetActorId() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Modified_Render_Component& evt);
};
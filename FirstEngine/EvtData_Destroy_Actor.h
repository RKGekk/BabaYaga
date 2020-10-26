#pragma once

#include <memory>
#include <string>

#include "IEventData.h"
#include "BaseEventData.h"
#include "Actor.h"

// Sent when actors are destroyed
class EvtData_Destroy_Actor : public BaseEventData {
    ActorId m_id;

public:
    static const EventTypeId sk_EventType = 0x77dd2b3a;
    static const std::string sk_EventName;

    explicit EvtData_Destroy_Actor(ActorId id = 0);

    virtual const EventTypeId& VGetEventType() const;
    virtual IEventDataPtr VCopy() const;
    virtual void VSerialize(std::ostream& out) const;
    virtual void VDeserialize(std::istream& in);
    virtual const std::string& GetName() const;

    ActorId GetId() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Destroy_Actor& evt);
};
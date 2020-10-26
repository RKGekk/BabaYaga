#pragma once

#include <DirectXMath.h>
#include <memory>
#include <string>

#include "BaseEventData.h"
#include "Actor.h"
#include "stringUtility.h"

// Sent when actors are moved
class EvtData_Move_Actor : public BaseEventData {
    ActorId m_id;
    DirectX::XMFLOAT4X4 m_matrix;

public:
    static const EventTypeId sk_EventType = 0xeeaa0a40;
    static const std::string sk_EventName;

    EvtData_Move_Actor();
    EvtData_Move_Actor(ActorId id, const DirectX::XMFLOAT4X4& matrix);

    virtual const EventTypeId& VGetEventType() const;
    virtual void VSerialize(std::ostream& out) const;
    virtual void VDeserialize(std::istream& in);
    virtual IEventDataPtr VCopy() const;
    virtual const std::string& GetName(void) const;

    ActorId GetId() const;
    const DirectX::XMFLOAT4X4& GetMatrix() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Move_Actor& evt);
};
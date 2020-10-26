#pragma once

#include "BaseEventData.h"
#include "Actor.h"

// This event is sent out when an actor is *actually* created.
class EvtData_New_Actor : public BaseEventData {
	ActorId m_actorId;
	unsigned int m_viewId;

public:
	static const EventTypeId sk_EventType = 0xe86c7c31;
	static const std::string sk_EventName;

	EvtData_New_Actor();
	explicit EvtData_New_Actor(ActorId actorId, unsigned long viewId = 0xffffffff);

	virtual void VDeserialize(std::istream& in);
	virtual const EventTypeId& VGetEventType() const;
	virtual IEventDataPtr VCopy() const;
	virtual void VSerialize(std::ostream& out) const;
	virtual const std::string& GetName() const;

	const ActorId GetActorId() const;
	unsigned int GetViewId() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_New_Actor& evt);
};
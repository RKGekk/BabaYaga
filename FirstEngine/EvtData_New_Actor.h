#pragma once

#include "BaseEventData.h"

// This event is sent out when an actor is *actually* created.
class EvtData_New_Actor : public BaseEventData {
	unsigned int m_actorId;
	unsigned int m_viewId;
	const std::string m_eventName;

public:
	static const EventTypeId sk_EventType = 0xe86c7c31;

	EvtData_New_Actor() : m_eventName("EvtData_New_Actor") {
		m_actorId = 0;
		m_viewId = 0;
	}

	explicit EvtData_New_Actor(unsigned long actorId, unsigned long viewId = 0xffffffff) : m_eventName("EvtData_New_Actor") {
		m_actorId = actorId;
		m_viewId = viewId;
	}

	virtual void VDeserialize(std::istream& in) {
		in >> m_actorId;
		in >> m_viewId;
	}

	virtual const EventTypeId& VGetEventType() const {
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy() const {
		return IEventDataPtr(new EvtData_New_Actor(m_actorId, m_viewId));
	}

	virtual void VSerialize(std::ostream& out) const {
		out << m_actorId << " ";
		out << m_viewId << " ";
	}

	virtual const std::string& GetName() const {
		return m_eventName;
	}

	const unsigned int GetActorId(void) const {
		return m_actorId;
	}

	unsigned int GetViewId(void) const {
		return m_viewId;
	}
};

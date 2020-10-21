#pragma once

#include <memory>

#include "BaseEventData.h"

// This event is sent when a new game is started
class EvtData_Environment_Loaded : public BaseEventData {
	const std::string m_eventName;
public:
	static const EventTypeId sk_EventType = 0x8E2AD6E6;

	EvtData_Environment_Loaded() : m_eventName("EvtData_Environment_Loaded") {}

	virtual const EventTypeId& VGetEventType() const {
		return sk_EventType;
	}

	virtual IEventDataPtr VCopy() const {
		return IEventDataPtr(new EvtData_Environment_Loaded());
	}

	virtual const std::string& GetName() const {
		return m_eventName;
	}
};
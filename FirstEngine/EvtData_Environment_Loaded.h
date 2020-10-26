#pragma once

#include <memory>
#include <string>

#include "BaseEventData.h"

// This event is sent when a new game is started
class EvtData_Environment_Loaded : public BaseEventData {
public:
	static const EventTypeId sk_EventType = 0x8E2AD6E6;
	static const std::string sk_EventName;

	EvtData_Environment_Loaded();

	virtual const EventTypeId& VGetEventType() const;
	virtual IEventDataPtr VCopy() const;
	virtual const std::string& GetName() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Environment_Loaded& evt);
};
#pragma once

#include "BaseEventData.h"

// this is sent by the authoritative game logic to all views so they will load a game level.
class EvtData_Request_Start : public BaseEventData {

public:
	static const EventTypeId sk_EventType = 0x11f2b19d;
	static const std::string sk_EventName;

	EvtData_Request_Start();

	virtual const EventTypeId& VGetEventType() const;
	virtual IEventDataPtr VCopy() const;
	virtual const std::string& GetName() const;
};
#pragma once

#include "IEventData.h"

class BaseEventData : public IEventData {
	const gameTimePoint m_timeStamp;

public:
	explicit BaseEventData(const gameTimePoint timeStamp = gameClock::now()) : m_timeStamp(timeStamp) {}

	// Returns the type of the event
	virtual const EventTypeId& VGetEventType() const = 0;

	gameTimePoint GetTimeStamp() const {
		return m_timeStamp;
	}

	// Serializing for network input / output
	virtual void VSerialize(std::ostream& out) const {}
	virtual void VDeserialize(std::istream& in) {}
};
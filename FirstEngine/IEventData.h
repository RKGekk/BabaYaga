#pragma once

#include <memory>
#include <iostream>
#include <string>

#include "FastDelegate.h"

#include "GameTimer.h"

class IEventData;

using EventTypeId = unsigned long;
using IEventDataPtr = std::shared_ptr<IEventData>;
using EventListenerDelegate = fastdelegate::FastDelegate1<IEventDataPtr>;

class IEventData {
public:
	virtual ~IEventData() {}
	virtual const EventTypeId& VGetEventType() const = 0;
	virtual gameTimePoint GetTimeStamp() const = 0;
	virtual void VSerialize(std::ostream& out) const = 0;
	virtual void VDeserialize(std::istream& in) = 0;
	virtual IEventDataPtr VCopy() const = 0;
	virtual const std::string& GetName() const = 0;
};
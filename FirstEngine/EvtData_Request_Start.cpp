#include "EvtData_Request_Start.h"

const std::string EvtData_Request_Start::sk_EventName = "EvtData_New_Actor";

EvtData_Request_Start::EvtData_Request_Start() {}

const EventTypeId& EvtData_Request_Start::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_Request_Start::VCopy() const {
	return IEventDataPtr(new EvtData_Request_Start());
}

const std::string& EvtData_Request_Start::GetName() const {
	return sk_EventName;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Request_Start& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os.flags(oldFlag);
	return os;
}
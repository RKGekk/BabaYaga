#pragma once

#include <DirectXMath.h>
#include <memory>
#include <string>

#include "BaseEventData.h"
#include "Actor.h"
#include "stringUtility.h"

class EvtData_Request_New_Actor : public BaseEventData {
	std::string m_actorResource;
	bool m_hasInitialTransform;
	DirectX::XMFLOAT4X4 m_initialTransform;
	ActorId m_serverActorId;

public:
	static const EventTypeId sk_EventType = 0x40378c64;
	static const std::string sk_EventName;

	EvtData_Request_New_Actor();
	explicit EvtData_Request_New_Actor(const std::string& actorResource, const DirectX::XMFLOAT4X4* initialTransform = NULL, const ActorId serverActorId = 0);

	virtual const EventTypeId& VGetEventType() const;
	virtual void VDeserialize(std::istream& in);
	virtual IEventDataPtr VCopy() const;
	virtual void VSerialize(std::ostream& out) const;
	virtual const std::string& GetName() const;

	const std::string& GetActorResource() const;
	const DirectX::XMFLOAT4X4* GetInitialTransform() const;
	const ActorId GetServerActorId() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Request_New_Actor& evt);
};
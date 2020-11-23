#pragma once

#include <memory>

#include "tinyxml.h"
#include "Actor.h"

class ActorComponent {
	friend class ActorFactory;

protected:
	StrongActorPtr m_pOwner;

public:
	virtual ~ActorComponent();

	virtual bool VInit(TiXmlElement* pData) = 0;
	virtual const std::string& VGetName() const = 0;

	virtual void VPostInit() {}
	virtual void VUpdate(float deltaMs) {}
	virtual void VOnChanged() {}
	virtual ComponentId VGetId() const;

	static ComponentId GetIdFromName(const std::string& componentStr);

	void SetOwner(StrongActorPtr pOwner);
	StrongActorPtr GetOwner();
	ActorId GetOwnerId();
};
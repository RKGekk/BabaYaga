#pragma once

#include <memory>

#include "tinyxml.h"
class Actor;

class ActorComponent {
	friend class ActorFactory;

protected:
	std::shared_ptr<Actor> m_pOwner;

public:
	virtual ~ActorComponent();

	virtual bool VInit(TiXmlElement* pData) = 0;
	virtual const char* VGetName() const = 0;

	virtual void VPostInit() {}
	virtual void VUpdate(float deltaMs) {}
	virtual void VOnChanged() {}
	virtual unsigned int VGetId() const;

	static unsigned int GetIdFromName(const char* componentStr);

	void SetOwner(std::shared_ptr<Actor> pOwner);
	std::shared_ptr<Actor> GetOwner();
	unsigned int GetOwnerId();
};
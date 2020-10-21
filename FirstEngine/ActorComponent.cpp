#include "ActorComponent.h"

#include "HashedString.h"
#include "Actor.h"

ActorComponent::~ActorComponent() {
	m_pOwner.reset();
}

unsigned int ActorComponent::GetIdFromName(const char* componentStr) {
	void* rawId = HashedString::hash_name(componentStr);
	return reinterpret_cast<unsigned int>(rawId);
}

unsigned int ActorComponent::VGetId() const {
	return GetIdFromName(VGetName());
}

void ActorComponent::SetOwner(std::shared_ptr<Actor> pOwner) {
	m_pOwner = pOwner;
}

std::shared_ptr<Actor> ActorComponent::GetOwner() {
	return m_pOwner;
}

unsigned int ActorComponent::GetOwnerId() {
	return m_pOwner->GetId();
}

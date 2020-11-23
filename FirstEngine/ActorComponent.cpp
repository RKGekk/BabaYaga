#include "ActorComponent.h"

#include "HashedString.h"
#include "Actor.h"

ActorComponent::~ActorComponent() {
	m_pOwner.reset();
}

ComponentId ActorComponent::GetIdFromName(const std::string& componentStr) {
	void* rawId = HashedString::hash_name(componentStr);
	return reinterpret_cast<ComponentId>(rawId);
}

ComponentId ActorComponent::VGetId() const {
	return GetIdFromName(VGetName());
}

void ActorComponent::SetOwner(StrongActorPtr pOwner) {
	m_pOwner = pOwner;
}

StrongActorPtr ActorComponent::GetOwner() {
	return m_pOwner;
}

ActorId ActorComponent::GetOwnerId() {
	return m_pOwner->GetId();
}

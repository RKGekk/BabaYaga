#include "Actor.h"
#include "ActorComponent.h"

Actor::Actor(ActorId id) {
    m_id = id;
    m_type = "Unknown";
    m_resource = "Unknown";
}

Actor::~Actor() {}

bool Actor::Init(TiXmlElement* pData) {
    m_type = pData->Attribute("type");
    m_resource = pData->Attribute("resource");
    return true;
}

void Actor::PostInit() {
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        it->second->VPostInit();
    }
}

void Actor::Destroy() {
    m_components.clear();
}

void Actor::Update(float deltaMs) {
    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        it->second->VUpdate(deltaMs);
    }
}

ActorId Actor::GetId() const {
    return m_id;
}

std::string Actor::GetType() const {
    return m_type;
}

const ActorComponents& Actor::GetComponents() {
    return m_components;
}

void Actor::AddComponent(StrongActorComponentPtr pComponent) {
    std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
}
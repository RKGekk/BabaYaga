#include "Actor.h"
#include "ActorComponent.h"

Actor::Actor(unsigned int id) {
    m_id = id;
    m_type = "Unknown";

    //added post press - this is an editor helper
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

void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent) {
    std::pair<std::map<unsigned int, std::shared_ptr<ActorComponent>>::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
}

const std::map<unsigned int, std::shared_ptr<ActorComponent>>* Actor::GetComponents() {
    return &m_components;
}
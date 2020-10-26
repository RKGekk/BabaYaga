#pragma once

#include <map>
#include <memory>
#include <string>

#include "tinyxml.h"

class Actor;
class ActorComponent;

using ActorId = unsigned int;
using ComponentId = unsigned int;

const ActorId INVALID_ACTOR_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

using StrongActorPtr = std::shared_ptr<Actor>;
using WeakActorPtr = std::weak_ptr<Actor>;
using StrongActorComponentPtr = std::shared_ptr<ActorComponent>;
using WeakActorComponentPtr = std::weak_ptr<ActorComponent>;
using ActorComponents = std::map<ComponentId, StrongActorComponentPtr>;

class Actor {

private:

    std::string     m_resource;     // the XML file from which this actor was initialized (considered the "Archetype" file)

    unsigned int    m_id;           // unique id for the actor
    std::string     m_type;
    std::map<unsigned int, std::shared_ptr<ActorComponent>> m_components;   // all components this actor has

public:
    explicit Actor(unsigned int id);
    ~Actor();

    bool Init(TiXmlElement* pData);
    void PostInit();
    void Destroy();
    void Update(float deltaMs);

    unsigned int GetId() const { return m_id; }
    std::string GetType() const { return m_type; }

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent(unsigned int id) {
        auto findIt = m_components.find(id);
        if (findIt != m_components.end()) {
            std::shared_ptr<ActorComponent> pBase(findIt->second);
            std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
            std::weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
            return pWeakSub;  // return the weak pointer
        }
        else {
            return std::weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    std::weak_ptr<ComponentType> GetComponent(const char* name) {
        unsigned int id = ActorComponent::GetIdFromName(name);
        auto findIt = m_components.find(id);
        if (findIt != m_components.end()) {
            std::shared_ptr<ActorComponent> pBase(findIt->second);
            std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
            std::weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
            return pWeakSub;  // return the weak pointer
        }
        else {
            return std::weak_ptr<ComponentType>();
        }
    }

    const std::map<unsigned int, std::shared_ptr<ActorComponent>>* GetComponents();

    void AddComponent(std::shared_ptr<ActorComponent> pComponent);
};
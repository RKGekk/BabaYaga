#pragma once

#include <memory>

#include "BaseEventData.h"
#include "SceneNode.h"

// This event is sent out when an actor is *actually* created.
class EvtData_New_Render_Component : public BaseEventData {
    unsigned int m_actorId;
    std::shared_ptr<SceneNode> m_pSceneNode;
    const std::string m_eventName;

public:
    static const EventTypeId sk_EventType = 0xaf4aff75;

    EvtData_New_Render_Component() : m_eventName("EvtData_New_Render_Component") {
        m_actorId = 0;
    }

    explicit EvtData_New_Render_Component(unsigned int actorId, std::shared_ptr<SceneNode> pSceneNode) : m_eventName("EvtData_New_Render_Component") {
        m_actorId = actorId;
        m_pSceneNode = pSceneNode;
    }

    virtual void VSerialize(std::ostream& out) const {}
    virtual void VDeserialize(std::istream& in) {}

    virtual const EventTypeId& VGetEventType() const {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const {
        return IEventDataPtr(new EvtData_New_Render_Component(m_actorId, m_pSceneNode));
    }

    virtual const std::string& GetName() const {
        return m_eventName;
    }

    const unsigned int GetActorId() const {
        return m_actorId;
    }

    std::shared_ptr<SceneNode> GetSceneNode() const {
        return m_pSceneNode;
    }
};
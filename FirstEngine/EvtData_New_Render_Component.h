#pragma once

#include <memory>
#include <string>

#include "BaseEventData.h"
#include "SceneNode.h"
#include "Actor.h"

// This event is sent out when an actor is *actually* created.
class EvtData_New_Render_Component : public BaseEventData {
    ActorId m_actorId;
    std::shared_ptr<SceneNode> m_pSceneNode;

public:
    static const EventTypeId sk_EventType = 0xaf4aff75;
    static const std::string sk_EventName;

    EvtData_New_Render_Component();
    explicit EvtData_New_Render_Component(ActorId actorId, std::shared_ptr<SceneNode> pSceneNode);

    virtual void VSerialize(std::ostream& out) const;
    virtual void VDeserialize(std::istream& in);
    virtual const EventTypeId& VGetEventType() const;
    virtual IEventDataPtr VCopy() const;
    virtual const std::string& GetName() const;

    ActorId GetActorId() const;
    std::shared_ptr<SceneNode> GetSceneNode() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_New_Render_Component& evt);
};
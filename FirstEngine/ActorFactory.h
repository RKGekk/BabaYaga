#pragma once

#include <map>
#include <string>
#include <memory>
#include <DirectXMath.h>

#include "tinyxml.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "GenericObjectFactory.h"
#include "memoryUtility.h"

class ActorFactory {

    unsigned int m_lastActorId;

protected:
    GenericObjectFactory<ActorComponent, unsigned int> m_componentFactory;

public:
    ActorFactory();

    std::shared_ptr<Actor> CreateActor(const char* actorResource, TiXmlElement* overrides, const DirectX::XMFLOAT4X4* initialTransform, const unsigned int serversActorId);
    void ModifyActor(std::shared_ptr<Actor> pActor, TiXmlElement* overrides);

    //protected:
        // This function can be overridden by a subclass so you can create game-specific C++ components.  If you do
        // this, make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
    virtual std::shared_ptr<ActorComponent> VCreateComponent(TiXmlElement* pData);

private:
    unsigned int GetNextActorId();
};
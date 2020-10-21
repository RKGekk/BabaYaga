#pragma once

#include <memory>
#include <map>
#include <DirectXMath.h>

#include "IEventData.h"
#include "Actor.h"
#include "ActorFactory.h"
#include "BaseEngineState.h"
#include "ProcessManager.h"
#include "IEventData.h"
#include "EvtData_Request_New_Actor.h"
#include "EvtData_Move_Actor.h"
#include "EvtData_Environment_Loaded.h"
#include "EvtData_Destroy_Actor.h"
#include "tinyxml.h"
#include "IEventManager.h"
#include "FastDelegate.h"
#include "BaseRenderComponent.h"
#include "TransformComponent.h"

class BaseEngineLogic {

protected:
	std::map<unsigned int, std::shared_ptr<Actor>>	m_actors;
	std::shared_ptr<ActorFactory>					m_pActorFactory;
	BaseEngineState									m_State;			// engine state: loading, running, etc.
	float											m_Lifetime;			//indicates how long this engine has been in session
	std::shared_ptr<ProcessManager>					m_pProcessManager;	// a engine logic entity

public:
	BaseEngineLogic();
	BaseEngineLogic(const BaseEngineLogic&) = delete;
	BaseEngineLogic& operator=(const BaseEngineLogic&) = delete;
	~BaseEngineLogic();
	bool Init();

	virtual void VOnUpdate(float time, float elapsedTime);

	virtual std::weak_ptr<Actor> VGetActor(const unsigned int actorId);
	virtual bool VLoadGame(const char* levelResource);
	virtual bool VLoadGameDelegate(TiXmlElement* pLevelData);

	virtual std::shared_ptr<Actor> VCreateActor(const std::string& actorResource, TiXmlElement* overrides, const DirectX::XMFLOAT4X4* initialTransform = NULL, const unsigned int serversActorId = 0);  // don't store this strong pointer outside of this class

	virtual void VMoveActor(const unsigned int id, const DirectX::XMFLOAT4X4& mat);

	void RegisterEngineEvents();

	void MoveActorDelegate(std::shared_ptr<IEventData> pEventData);
	void RequestNewActorDelegate(std::shared_ptr<IEventData> pEventData);

	BaseEngineState GetEngineState();
	void SetEngineState(BaseEngineState es);
	virtual void VChangeState(BaseEngineState newState);
};
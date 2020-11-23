#include "BaseEngineLogic.h"
#include "EvtData_New_Actor.h"

#include <iostream>

BaseEngineLogic::BaseEngineLogic() {
	m_State = BaseEngineState::BGS_Invalid;
	m_Lifetime = 0.0f;
}

BaseEngineLogic::~BaseEngineLogic() {

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &BaseEngineLogic::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
}

bool BaseEngineLogic::Init() {
	m_pActorFactory.reset(new ActorFactory);
	m_pProcessManager.reset(new ProcessManager);

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &BaseEngineLogic::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);

	return true;
}

void BaseEngineLogic::VOnUpdate(float time, float elapsedTime) {

	int deltaMilliseconds = int(elapsedTime * 1000.0f);
	m_Lifetime += elapsedTime;

	switch (m_State) {
		case BaseEngineState::BGS_Invalid:
		break;

		case BaseEngineState::BGS_Initializing:
		break;

		case BaseEngineState::BGS_LoadingGameEnvironment:
		break;

		case BaseEngineState::BGS_Running: {
			m_pProcessManager->UpdateProcesses(deltaMilliseconds);
		}
		break;

		default:
		break;
	}

	// update game actors
	for (auto it = m_actors.begin(); it != m_actors.end(); ++it) {
		it->second->Update(deltaMilliseconds);
	}

}

std::weak_ptr<Actor> BaseEngineLogic::VGetActor(const unsigned int actorId) {
	auto findIt = m_actors.find(actorId);
	if (findIt != m_actors.end()) {
		return findIt->second;
	}
	return std::weak_ptr<Actor>();
}

bool BaseEngineLogic::VLoadGame(const char* levelResource) {

	TiXmlDocument xml;
	xml.LoadFile(levelResource);

	// Grab the root XML node
	TiXmlElement* pRoot = xml.RootElement();
	if (!pRoot) {
		return false;
	}

	// load all initial actors
	TiXmlElement* pActorsNode = pRoot->FirstChildElement("StaticActors");
	if (pActorsNode) {
		for (TiXmlElement* pNode = pActorsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
			const char* actorResource = pNode->Attribute("resource");
			std::shared_ptr<Actor> pActor = VCreateActor(actorResource, pNode);
			if (pActor) {
				// fire an event letting everyone else know that we created a new actor
				std::shared_ptr<EvtData_New_Actor> pNewActorEvent(new EvtData_New_Actor(pActor->GetId()));
				IEventManager::Get()->VQueueEvent(pNewActorEvent);
			}
		}
	}

	// call the delegate load function
	if (!VLoadGameDelegate(pRoot)) {
		return false;  // no error message here because it's assumed VLoadGameDelegate() kicked out the error
	}
	return true;
}

bool BaseEngineLogic::VLoadGameDelegate(TiXmlElement* pLevelData) {

	return true;
}

std::shared_ptr<Actor> BaseEngineLogic::VCreateActor(const std::string& actorResource, TiXmlElement* overrides, const DirectX::XMFLOAT4X4* initialTransform, const unsigned int serversActorId) {
	std::shared_ptr<Actor> pActor = m_pActorFactory->CreateActor(actorResource.c_str(), overrides, initialTransform, serversActorId);
	if (pActor) {
		m_actors.insert(std::make_pair(pActor->GetId(), pActor));
		if (m_State == BaseEngineState::BGS_Running) {
			std::shared_ptr<EvtData_Request_New_Actor> pNewActor(new EvtData_Request_New_Actor(actorResource, initialTransform, pActor->GetId()));
			IEventManager::Get()->VTriggerEvent(pNewActor);
		}
		return pActor;
	}
	else {
		// FUTURE WORK: Log error: couldn't create actor
		return std::shared_ptr<Actor>();
	}
}

void BaseEngineLogic::VMoveActor(const unsigned int id, const DirectX::XMFLOAT4X4& mat) {

	std::shared_ptr<Actor> act = MakeStrongPtr(VGetActor(id));
	unsigned int componentId = ActorComponent::GetIdFromName("TransformComponent");
	std::shared_ptr<TransformComponent> rc = MakeStrongPtr(act->GetComponent<TransformComponent>(componentId));
	rc->SetTransform(mat);

	//std::shared_ptr<EvtData_Move_Actor> pNewActorEvent(new EvtData_Move_Actor(1, mat));
	//IEventManager::Get()->VQueueEvent(pNewActorEvent);
}

void BaseEngineLogic::MoveActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Move_Actor> pCastEventData = std::static_pointer_cast<EvtData_Move_Actor>(pEventData);
	VMoveActor(pCastEventData->GetId(), pCastEventData->GetMatrix());
}

void BaseEngineLogic::RequestNewActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::cout << "Hi from event" << std::endl;

	std::shared_ptr<EvtData_New_Actor> act = std::static_pointer_cast<EvtData_New_Actor>(pEventData);
}

BaseEngineState BaseEngineLogic::GetEngineState() {
	return m_State;
}

void BaseEngineLogic::SetEngineState(BaseEngineState es) {
	m_State = es;
}

void BaseEngineLogic::VChangeState(BaseEngineState newState) {
	m_State = newState;
}


void BaseEngineLogic::RegisterEngineEvents() {
	REGISTER_EVENT(EvtData_Environment_Loaded);
	REGISTER_EVENT(EvtData_New_Actor);
	REGISTER_EVENT(EvtData_Move_Actor);
	REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
}


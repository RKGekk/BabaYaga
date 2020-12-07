#include "SceneTree.h"

#include "RootNode.h"
#include "ISceneNode.h"
#include "IEventData.h"
#include "IEventManager.h"
#include "FastDelegate.h"
#include "memoryUtility.h"
#include "stringUtility.h"
#include "EvtData_Modified_Render_Component.h"
#include "EvtData_New_Render_Component.h"
#include "EvtData_Destroy_Actor.h"
#include "EvtData_Move_Actor.h"
#include "BaseEngineLogic.h"
#include "MatrixStack.h"
#include "Actor.h"
#include "CameraNode.h"
#include "FreeCameraNode.h"
#include "SystemClass.h"
#include "LightManager.h"

SceneTree::SceneTree() {

	m_Root.reset(new RootNode());
	m_MatrixStack = std::shared_ptr<MatrixStack>(new MatrixStack());
	m_LightManager = std::shared_ptr<LightManager>(new LightManager());

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
}

SceneTree::~SceneTree() {

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
}

HRESULT SceneTree::OnRender(ID3D11DeviceContext* deviceContext) {
	// The render passes usually go like this 
	// 1. Static objects & terrain
	// 2. Actors (dynamic objects that can move)
	// 3. The Sky
	// 4. Anything with Alpha
	if (m_Root && m_Camera) {
		// The scene root could be anything, but it
		// is usually a SceneNode with the identity
		// matrix

		m_Camera->VRender(this, deviceContext);
		m_LightManager->CalcLighting(this);

		if (m_Root->VPreRender(this, deviceContext) == S_OK) {
			m_Root->VRender(this, deviceContext);
			m_Root->VRenderChildren(this, deviceContext);
			m_Root->VPostRender(this);
		}
	}

	return S_OK;
}

HRESULT SceneTree::OnUpdate(const float deltaMilliseconds, ID3D11DeviceContext* deviceContext) {
	if (!m_Root) {
		return S_OK;
	}

	DWORD elapsedTime = 10;
	return m_Root->VOnUpdate(this, elapsedTime, deviceContext);
}

std::shared_ptr<ISceneNode> SceneTree::FindSceneNode(unsigned int actorId) {
	auto i = m_ActorMap.find(actorId);
	if (i == m_ActorMap.end()) {
		return std::shared_ptr<ISceneNode>();
	}

	return i->second;
}

std::shared_ptr<Actor> SceneTree::FindActor(unsigned int actorId) {
	return MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actorId));
	//return std::shared_ptr<Actor>();

	//auto i = m_ActorMap.find(actorId);
	//if (i == m_ActorMap.end()) {
	//	return std::shared_ptr<Actor>();
	//}
	//
	//return i->second;
}

bool SceneTree::AddChild(unsigned int actorId, std::shared_ptr<ISceneNode> kid) {
	if (actorId != 0) {
		// This allows us to search for this later based on actor id
		m_ActorMap[actorId] = kid;
	}

	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL && m_LightManager->m_Lights.size() + 1 <= MAXIMUM_LIGHTS_SUPPORTED) {
		m_LightManager->m_Lights.push_back(pLight);
	}

	return m_Root->VAddChild(kid);
}

bool SceneTree::RemoveChild(unsigned int actorId) {
	if (actorId == 0) {
		return false;
	}
	std::shared_ptr<ISceneNode> kid = FindSceneNode(actorId);
	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL) {
		m_LightManager->m_Lights.remove(pLight);
	}
	m_ActorMap.erase(actorId);
	return m_Root->VRemoveChild(actorId);
}

void SceneTree::NewRenderComponentDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_New_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Render_Component>(pEventData);
	unsigned int actorId = pCastEventData->GetActorId();
	std::shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());
	AddChild(actorId, pSceneNode);
}

void SceneTree::ModifiedRenderComponentDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Modified_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_Modified_Render_Component>(pEventData);

	unsigned int actorId = pCastEventData->GetActorId();
	if (actorId == 0) {
		return;
	}

	if (SystemClass::GetEngineLogic()->GetEngineState() == BaseEngineState::BGS_LoadingGameEnvironment) {
		return;
	}

	std::shared_ptr<ISceneNode> pSceneNode = FindSceneNode(actorId);
	if (pSceneNode) {
		pSceneNode->VOnRestore(this);
	}
}

void SceneTree::DestroyActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
	RemoveChild(pCastEventData->GetId());
}

void SceneTree::MoveActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Move_Actor> pCastEventData = std::static_pointer_cast<EvtData_Move_Actor>(pEventData);

	unsigned int actorId = pCastEventData->GetId();
	DirectX::XMFLOAT4X4 transform = pCastEventData->GetMatrix();

	std::shared_ptr<ISceneNode> pNode = FindSceneNode(actorId);
	if (pNode) {
		pNode->VSetTransform(&transform);
	}
}

void SceneTree::SetCamera(std::shared_ptr<FreeCameraNode> camera) {
	m_Camera = camera;
}

const std::shared_ptr<FreeCameraNode> SceneTree::GetCamera() const {
	return m_Camera;
}

void SceneTree::PushAndSetMatrix(const DirectX::XMFLOAT4X4& toWorld) {
	m_MatrixStack->Push();
	m_MatrixStack->MultMatrixLocal(toWorld);
}

void SceneTree::PopMatrix() {
	m_MatrixStack->Pop();
}

const DirectX::XMFLOAT4X4 SceneTree::GetTopMatrix() {
	return m_MatrixStack->GetTop();
}

const DirectX::XMFLOAT4X4 SceneTree::GetTopMatrixT() {
	return m_MatrixStack->GetTopT();
}

std::shared_ptr<LightManager> SceneTree::GetLightManager() {
	return m_LightManager;
}
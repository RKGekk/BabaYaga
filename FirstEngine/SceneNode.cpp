#include "SceneNode.h"

#include "SceneTree.h"
#include "memoryUtility.h"
#include "BaseEngineLogic.h"
#include "TransformComponent.h"
#include "BaseRenderComponent.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>

SceneNode::SceneNode(BaseRenderComponent* renderComponent, RenderPass renderPass, DirectX::XMFLOAT4X4* to, DirectX::XMFLOAT4X4* from) {

	DirectX::XMFLOAT4X4 to4x4;
	if (to == nullptr) {
		DirectX::XMStoreFloat4x4(&to4x4, DirectX::XMMatrixIdentity());
	}
	else {
		to4x4 = *to;
	}

	VSetTransform(&to4x4, from);
	SetRadius(0);
	m_RenderComponent = renderComponent;

	m_pParent = nullptr;

	m_Props.m_ActorId = (renderComponent) ? renderComponent->GetOwnerId() : 0;
	m_Props.m_Name = (renderComponent) ? renderComponent->VGetName() : "SceneNode";
	m_Props.m_RenderPass = renderPass;
}

const SceneNodeProperties* const SceneNode::VGet() const {
	return &m_Props;
}

SceneNode::~SceneNode() {}

void SceneNode::SetPosition(const DirectX::XMFLOAT3& pos) {

	m_Props.m_ToWorld.m[3][0] = pos.x;
	m_Props.m_ToWorld.m[3][1] = pos.y;
	m_Props.m_ToWorld.m[3][2] = pos.z;
	m_Props.m_ToWorld.m[3][3] = 1.0f;
}

// This was added post press to respect any SceneNode ancestors - you have to add all 
// their positions together to get the world position of any SceneNode.
const DirectX::XMFLOAT3 SceneNode::GetWorldPosition() const {
	DirectX::XMFLOAT3 pos = GetPosition();
	if (m_pParent) {
		DirectX::XMFLOAT3 wp1 = m_pParent->GetWorldPosition();
		pos.x += wp1.x;
		pos.y += wp1.y;
		pos.z += wp1.z;
	}
	return pos;
}

DirectX::XMFLOAT3 SceneNode::GetPosition() const {
	return DirectX::XMFLOAT3(m_Props.m_ToWorld.m[3][0], m_Props.m_ToWorld.m[3][1], m_Props.m_ToWorld.m[3][2]);
}

DirectX::XMFLOAT3 SceneNode::GetDirection() const {

	DirectX::XMFLOAT4X4 justRot4x4 = m_Props.m_ToWorld;
	justRot4x4.m[3][0] = 0.0f;
	justRot4x4.m[3][1] = 0.0f;
	justRot4x4.m[3][2] = 0.0f;
	justRot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX justRot = DirectX::XMLoadFloat4x4(&justRot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR out = DirectX::XMVector4Transform(forward, justRot);
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, out);

	return result;
}

void SceneNode::SetRadius(const float radius) {
	m_Props.m_Radius = radius;
}

HRESULT SceneNode::VOnUpdate(SceneTree* pScene, float const elapsedMs, ID3D11DeviceContext* deviceContext) {
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnUpdate()

	auto i = m_Children.begin();
	auto end = m_Children.end();

	while (i != end) {
		(*i)->VOnUpdate(pScene, elapsedMs, deviceContext);
		++i;
	}
	return S_OK;
}

void SceneNode::VSetTransform(DirectX::XMFLOAT4X4* toWorld, DirectX::XMFLOAT4X4* fromWorld) {
	m_Props.m_ToWorld = *toWorld;
	if (!fromWorld) {
		DirectX::XMMATRIX toWorldMatrix = DirectX::XMLoadFloat4x4(&m_Props.m_ToWorld);
		DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(toWorldMatrix), toWorldMatrix);
		DirectX::XMStoreFloat4x4(&m_Props.m_FromWorld, invWorld);
	}
	else {
		m_Props.m_FromWorld = *fromWorld;
	}
}

HRESULT SceneNode::VPreRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	// This was added post press! Is is always ok to read directly from the game logic.
	std::shared_ptr<Actor> pActor = pScene->FindActor(m_Props.m_ActorId);
	if (pActor) {
		std::shared_ptr<TransformComponent> pTc = MakeStrongPtr(pActor->GetComponent<TransformComponent>("TransformComponent"));
		if (pTc) {
			m_Props.m_ToWorld = pTc->GetTransform4x4f();
		}
	}

	pScene->PushAndSetMatrix(m_Props.m_ToWorld);
	return S_OK;
}

HRESULT SceneNode::VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {

	return S_OK;
}

HRESULT SceneNode::VRenderChildren(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	// Iterate through the children....
	auto i = m_Children.begin();
	auto end = m_Children.end();

	while (i != end) {
		if ((*i)->VPreRender(pScene, deviceContext) == S_OK) {
			// You could short-circuit rendering
			// if an object returns E_FAIL from
			// VPreRender()

			(*i)->VRender(pScene, deviceContext);

			// see comment just below...
			(*i)->VRenderChildren(pScene, deviceContext);

			// fix - if the parent is not visible, the childrend
			// shouldn't be visible either.
			//(*i)->VRenderChildren(pScene);
		}
		(*i)->VPostRender(pScene);
		++i;
	}

	return S_OK;
}

HRESULT SceneNode::VPostRender(SceneTree* pScene) {
	pScene->PopMatrix();
	return S_OK;
}

bool SceneNode::VAddChild(std::shared_ptr<ISceneNode> ikid) {
	m_Children.push_back(ikid);

	std::shared_ptr<SceneNode> kid = std::static_pointer_cast<SceneNode>(ikid);

	kid->m_pParent = this;					// [mrmike] Post-press fix - the parent was never set!

	// The radius of the sphere should be fixed right here
	DirectX::XMFLOAT4X4 ToWorld = kid->VGet()->ToWorld();
	DirectX::XMFLOAT3 kidPos(ToWorld.m[3][0], ToWorld.m[3][1], ToWorld.m[3][2]);

	// Fix. This was not correct! subtracting the parents's position from the kidPos
	// created a HUGE radius, depending on the location of the parent, which could be anywhere
	// in the game world.

	DirectX::XMVECTOR len = DirectX::XMVector3Length(DirectX::XMLoadFloat3(&kidPos));
	DirectX::XMFLOAT3 lenF;
	DirectX::XMStoreFloat3(&lenF, len);
	float newRadius = lenF.x + kid->VGet()->Radius();

	if (newRadius > m_Props.m_Radius) {
		m_Props.m_Radius = newRadius;
	}

	return true;
}

// If an actor is destroyed it should be removed from the scene graph.
// Generally the HumanView will recieve a message saying the actor has been
// destroyed, and it will then call Scene::RemoveChild which will traverse 
// the scene graph to find the child that needs removing.
bool SceneNode::VRemoveChild(unsigned int actorid) {
	for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
		const SceneNodeProperties* pProps = (*i)->VGet();
		if (pProps->ActorId() != 0 && actorid == pProps->ActorId()) {
			i = m_Children.erase(i);	//this can be expensive for vectors
			return true;
		}
	}
	return false;
}
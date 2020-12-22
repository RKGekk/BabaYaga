#include "Scene3.h"

#include <memory>

#include "FreeCameraNode.h"
#include "SystemClass.h"
#include "memoryUtility.h"


Scene3::Scene3(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Scene("First s") {

	m_Cam = std::shared_ptr<FreeCameraNode>(new FreeCameraNode());
	m_Cam->SetPosition(0.0f, 2.0f, 0.0f);
	
	//m_Cam->SetPosition(0.0f, 2.0f, 0.0f);
	m_Cam->Pitch(-DirectX::XM_PI);
	m_sceneTree = std::make_unique<SceneTree>(options, device, deviceContext);
	m_sceneTree->SetCamera(m_Cam);
}

void Scene3::Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) {


	//m_Cam->SetPosition(96.0f - 10.0f, 2.0f, 0.0f);

	// scene1
	if (tt < 16.5f) {
		m_Cam->SetPosition(0.0f, 2.0f, -16.0f + tt);
		m_Cam->Pitch(DirectX::XM_PI * -0.0625f * dt);
	}

	// scene2
	static float pos = -32.0f;
	if (tt >= 16.5f) {
		pos += dt * 16.0f;
		if (pos >= 10.0f) {
			pos -= 43.0f;
		}
		m_Cam->SetPosition(96.0f - 10.0f, 2.0f, pos);
	}

	unsigned int actId = 5;
	std::shared_ptr<Actor> act = MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actId));
	if (act) {
		unsigned int componentId = ActorComponent::GetIdFromName("TransformComponent");
		std::shared_ptr<TransformComponent> rc = MakeStrongPtr(act->GetComponent<TransformComponent>(componentId));
		float factorX = rc->GetPosition3f().x;
		float factorY = rc->GetPosition3f().y;
		float factorZ = rc->GetPosition3f().z;
		float factorYaw = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().x) + dt * 16.0f;
		float factorPitch = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().y);
		float factorRoll = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().z);
		DirectX::XMFLOAT4X4 m_testMatrix;
		DirectX::XMStoreFloat4x4(
			&m_testMatrix,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(factorPitch), DirectX::XMConvertToRadians(factorYaw), DirectX::XMConvertToRadians(factorRoll)),
				DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(factorX, factorY, factorZ))
			)
		);
		std::shared_ptr<EvtData_Move_Actor> pMoveActorEvent(new EvtData_Move_Actor(actId, m_testMatrix));
		IEventManager::Get()->VQueueEvent(pMoveActorEvent);
	}

	unsigned int actId4 = 14;
	std::shared_ptr<Actor> act4 = MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actId4));
	if (act4) {
		unsigned int componentId = ActorComponent::GetIdFromName("TransformComponent");
		std::shared_ptr<TransformComponent> rc = MakeStrongPtr(act4->GetComponent<TransformComponent>(componentId));
		float factorX = 86.0f;
		float factorY = 2.0f;
		float factorZ = pos + 2.0f;
		float factorYaw = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().x) + dt * 16.0f;
		float factorPitch = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().y);
		float factorRoll = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().z);
		DirectX::XMFLOAT4X4 m_testMatrix;
		DirectX::XMStoreFloat4x4(
			&m_testMatrix,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(factorPitch), DirectX::XMConvertToRadians(factorYaw), DirectX::XMConvertToRadians(factorRoll)),
				DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(factorX, factorY, factorZ))
			)
		);
		std::shared_ptr<EvtData_Move_Actor> pMoveActorEvent(new EvtData_Move_Actor(actId4, m_testMatrix));
		IEventManager::Get()->VQueueEvent(pMoveActorEvent);
	}

	unsigned int actId2 = 12;
	std::shared_ptr<Actor> act2 = MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actId2));
	if (act2) {
		unsigned int componentId = ActorComponent::GetIdFromName("TransformComponent");
		std::shared_ptr<TransformComponent> rc = MakeStrongPtr(act2->GetComponent<TransformComponent>(componentId));
		float factorX = 86.0f;
		float factorY = 2.0f;
		float factorZ = pos + 2.0f;
		float factorYaw = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().x);
		float factorPitch = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().y);
		float factorRoll = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().z) + dt * 64.0f;
		DirectX::XMFLOAT4X4 m_testMatrix;
		DirectX::XMStoreFloat4x4(
			&m_testMatrix,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(factorPitch), DirectX::XMConvertToRadians(factorYaw), DirectX::XMConvertToRadians(factorRoll)),
				DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(factorX, factorY, factorZ))
			)
		);
		std::shared_ptr<EvtData_Move_Actor> pMoveActorEvent(new EvtData_Move_Actor(actId2, m_testMatrix));
		IEventManager::Get()->VQueueEvent(pMoveActorEvent);
	}

	unsigned int actId3 = 13;
	std::shared_ptr<Actor> act3 = MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actId3));
	if (act3) {
		unsigned int componentId = ActorComponent::GetIdFromName("TransformComponent");
		std::shared_ptr<TransformComponent> rc = MakeStrongPtr(act3->GetComponent<TransformComponent>(componentId));
		float factorX = 86.0f;
		float factorY = 2.0f;
		float factorZ = pos + 2.0f;
		float factorYaw = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().x);
		float factorPitch = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().y);
		float factorRoll = DirectX::XMConvertToDegrees(rc->GetYawPitchRoll3f().z) + dt * 64.0f;
		DirectX::XMFLOAT4X4 m_testMatrix;
		DirectX::XMStoreFloat4x4(
			&m_testMatrix,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(factorPitch), DirectX::XMConvertToRadians(factorYaw), DirectX::XMConvertToRadians(factorRoll)),
				DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(factorX, factorY, factorZ))
			)
		);
		std::shared_ptr<EvtData_Move_Actor> pMoveActorEvent(new EvtData_Move_Actor(actId3, m_testMatrix));
		IEventManager::Get()->VQueueEvent(pMoveActorEvent);
	}

	//float fSpeed = 2.0f;

	//if (kbd.IsKeyDown(16)) {
	//	fSpeed = 16.0f;
	//}
	//if (kbd.IsKeyDown('W')) {
	//	m_camera->Walk(fSpeed * dt);
	//}
	//if (kbd.IsKeyDown('A')) {
	//	m_camera->Strafe(fSpeed * -dt);
	//}
	//if (kbd.IsKeyDown('S')) {
	//	m_camera->Walk(fSpeed * -dt);
	//}
	//if (kbd.IsKeyDown('D')) {
	//	m_camera->Strafe(fSpeed * dt);
	//}

	/*while (!mouse.IsEmpty()) {
		const auto e = mouse.Read();
		switch (e.GetType()) {
			case Mouse::Event::Type::LPress:
			m_MouseTracker.Engage(e.GetPosX(), e.GetPosY());
			break;
			case Mouse::Event::Type::LRelease:
			m_MouseTracker.Release();
			break;
			case Mouse::Event::Type::Move:
			if (m_MouseTracker.Engaged()) {
				const auto delta = m_MouseTracker.Move(e.GetPosX(), e.GetPosY());
				m_camera->Pitch(1.0f * (((float)delta.y) * 0.5f * DirectX::XM_PI) / 180.0f);
				m_camera->RotateY(1.0f * (((float)delta.x) * 0.5f * DirectX::XM_PI) / 180.0f);
			}
			break;
		}
	}*/


	m_sceneTree->OnUpdate(dt, deviceContext);
}

HRESULT Scene3::OnRestore(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	return m_sceneTree->OnRestore(options, device, deviceContext);
}

void Scene3::Draw(ID3D11DeviceContext* deviceContext) {
	m_sceneTree->OnRender(deviceContext);
}
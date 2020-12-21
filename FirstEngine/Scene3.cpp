#include "Scene3.h"

#include "FreeCameraNode.h"

Scene3::Scene3(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Scene("First s") {

	std::shared_ptr<FreeCameraNode> pCam = std::shared_ptr<FreeCameraNode>(new FreeCameraNode());
	pCam->SetPosition(0.0f, 2.0f, 0.0f);
	m_sceneTree = std::make_unique<SceneTree>(options, device, deviceContext);
	m_sceneTree->SetCamera(pCam);
}

void Scene3::Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) {

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
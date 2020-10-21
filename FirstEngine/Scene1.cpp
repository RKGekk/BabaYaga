#include "Scene1.h"

Scene1::Scene1(ID3D11Device* device) : Scene("First s") {

	//float aspect_ratio = 1.3333f;
	float aspect_ratio = 1.7777f;
	float hfov = 65.0f;
	float vfov = hfov / aspect_ratio;

	m_camera = std::make_unique<CameraClass>();
	//m_camera->SetPosition(-25.0f, 1.5f, -25.0f);
	m_camera->SetPosition(0.0f, 2.0f, 0.0f);
	//m_camera->Pitch(DirectX::XM_PI / 4.0f); 
	m_camera->SetLens(DirectX::XMConvertToRadians(hfov), aspect_ratio, 0.1f, 1000.0f);
	m_camera->UpdateViewMatrix();

	std::wstring skyTexture = L"10.dds";
	m_sky = std::make_unique<Sky>(device, skyTexture, m_camera.get());

	DirectX::XMStoreFloat4x4(&m_moonMatrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
	m_moon = std::make_unique<MoonCrate>(device, m_camera.get(), &m_moonMatrix);
}

void Scene1::Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) {

	float fSpeed = 2.0f;

	if (kbd.IsKeyDown(16)) {
		fSpeed = 16.0f;
	}
	if (kbd.IsKeyDown('W')) {
		m_camera->Walk(fSpeed * dt);
	}
	if (kbd.IsKeyDown('A')) {
		m_camera->Strafe(fSpeed * -dt);
	}
	if (kbd.IsKeyDown('S')) {
		m_camera->Walk(fSpeed * -dt);
	}
	if (kbd.IsKeyDown('D')) {
		m_camera->Strafe(fSpeed * dt);
	}

	while (!mouse.IsEmpty()) {
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
	}

	m_camera->UpdateViewMatrix();
	m_sky->Update(deviceContext, dt, tt);
	m_moon->Update(deviceContext, dt, tt);
}

void Scene1::Draw(ID3D11DeviceContext* deviceContext) {

	m_moon->Draw(deviceContext);
	m_sky->Draw(deviceContext);
}

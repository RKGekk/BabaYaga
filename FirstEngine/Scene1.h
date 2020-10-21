#pragma once

#include <string>
#include <memory>
#include <d3d11.h>

#include "Scene.h"

#include "CameraClass.h"
#include "TextureHolder.h"
#include "TextureClass.h"

#include "Sky.h"
#include "MoonCrate.h"

class Scene1 : Scene {

public:
	Scene1(ID3D11Device* device);

	virtual void Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext) override;

private:
	std::unique_ptr<Sky>			m_sky;

	std::unique_ptr<MoonCrate>		m_moon;
	DirectX::XMFLOAT4X4				m_moonMatrix;

	std::unique_ptr<CameraClass>	m_camera;

	Mouse::MouseTracker				m_MouseTracker;
};
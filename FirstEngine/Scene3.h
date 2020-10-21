#pragma once

#include <string>
#include <memory>
#include <d3d11.h>

#include "Scene.h"
#include "CameraClass.h"
#include "TextureHolder.h"
#include "TextureClass.h"
#include "SceneTree.h"

class Scene3 : Scene {

public:
	Scene3(ID3D11Device* device);

	virtual void Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext) override;

private:
	
	Mouse::MouseTracker				m_MouseTracker;
	std::unique_ptr<SceneTree>		m_sceneTree;
};
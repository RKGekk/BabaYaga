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
	Scene3(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	virtual void Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) override;
	virtual HRESULT OnRestore(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext) override;

private:
	
	Mouse::MouseTracker				m_MouseTracker;
	std::unique_ptr<SceneTree>		m_sceneTree;
	std::shared_ptr<FreeCameraNode> m_Cam;
};
#pragma once

#include <Windows.h>
#include <memory>

#include "D3DClass.h"
#include "EngineOptions.h"
#include "TextureHolder.h"
#include "TextureShaderClass.h"
#include "ShaderClass.h"
#include "ShaderHolder.h"
#include "CameraClass.h"
#include "GameTimer.h"
#include "ModelClass.h"
#include "GDIPlusManager.h"
#include "Scene1.h"
#include "Scene3.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "EvtData_Move_Actor.h"
#include "IEventManager.h"
#include "FastDelegate.h"

class GraphicsClass {
public:
	GraphicsClass(const EngineOptions& options);
	GraphicsClass(const GraphicsClass&) = delete;
	GraphicsClass& operator=(const GraphicsClass&) = delete;
	~GraphicsClass();

	bool Initialize(const EngineOptions& options, HWND hwnd);
	void Shutdown();

	void OnResize(const EngineOptions& options);

	void Update(InputClass& kbd, Mouse& mouse, const GameTimer&);
	bool Frame();
	void DrawIndexed(int count);

	float AspectRatio() const;

	std::shared_ptr<D3DClass>			GetD3D();
	std::shared_ptr<TextureHolder>		GetTexture();

	int GetWidth();
	int GetHeight();

private:
	bool Render();

	GDIPlusManager								gdipMan;

	std::shared_ptr<D3DClass>					m_Direct3D;
	std::shared_ptr<TextureHolder>				m_TextureHolder;
	std::shared_ptr<ShaderHolder>				m_ShaderHolder;

	std::unique_ptr<Scene1>						m_Scene1;
	std::unique_ptr<Scene3>						m_Scene3;
	int actId = 0;
	float factorX = 0.0f;
	float factorY = 0.0f;
	float factorZ = 0.0f;
	float factorYaw = 0.0f;
	float factorPitch = 0.0f;
	float factorRoll = 0.0f;
	float col2Ambient[4];
	float col2Diffuse[4];
	float col2Specular[4];
	//int											m_sceneNumber;

	int											mClientWidth;
	int											mClientHeight;
};
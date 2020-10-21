#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "winmm.lib")

#include <algorithm>
#include <windows.h>
#include <Mmsystem.h>
#include <memory>

#include "inputclass.h"
#include "Mouse.h"
#include "GameTimer.h"
#include "EngineOptions.h"
#include "GraphicsClass.h"
#include "BaseEngineLogic.h"
#include "BaseEngineState.h"
#include "EventManager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class SystemClass {
public:
	SystemClass();
	SystemClass(const SystemClass&) = delete;
	SystemClass& operator=(const SystemClass&) = delete;
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	void OnResize();

	static Renderer GetRendererType();
	static GraphicsClass& GetGraphics();
	static EngineOptions& GetEngineOptions();
	static BaseEngineLogic* GetEngineLogic();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);	

private:
	bool Frame();
	void Update(const GameTimer&);
	bool InitializeWindows();
	void ShutdownWindows();

	std::unique_ptr<EventManager>		m_pEventManager;

	static SystemClass* m_applicationHandle;

	LPCWSTR				m_applicationName;
	HINSTANCE			m_hinstance;
	HWND				m_hwnd;

	bool				m_AppPaused;
	bool				m_Minimized;
	bool				m_Maximized;
	bool				m_Resizing;
	EngineOptions		m_engineOptions;

	GameTimer			m_Timer;
	InputClass			m_Input;
	Mouse				m_Mouse;

	GraphicsClass		m_Graphics;

	std::unique_ptr<BaseEngineLogic>	m_pGame;
};
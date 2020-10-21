#include "SystemClass.h"

//static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

SystemClass* SystemClass::m_applicationHandle = nullptr;

SystemClass::SystemClass() : 
	m_applicationName(L"Engine"),
	m_hinstance(),
	m_hwnd(),
	m_AppPaused(false),
	m_Minimized(false),
	m_Maximized(false),
	m_Resizing(false),
	m_Graphics(m_engineOptions),
	m_Timer(),
	m_Input() {

	// Get an external pointer to this object.
	m_applicationHandle = this;

	m_pGame = nullptr;
	m_pEventManager = nullptr;
}

SystemClass::~SystemClass() {}

bool SystemClass::Initialize() {
	bool result;

	if (!XMVerifyCPUSupport()) {
		return false;
	}

	m_engineOptions.Init("EngineOptions.xml", nullptr);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Initialize the windows api.
	if (!InitializeWindows()) {
		return false;
	}

	// Initialize the input object.
	m_Input.Initialize();

	m_pEventManager = std::make_unique<EventManager>("GameCodeApp Event Mgr", true);

	// Initialize the graphics object.
	result = m_Graphics.Initialize(m_engineOptions, m_hwnd);
	if (!result) {
		return false;
	}

	m_pGame = std::make_unique<BaseEngineLogic>();
	m_pGame->VChangeState(BaseEngineState::BGS_Initializing);
	result = m_pGame->Init();
	if (!result) {
		return false;
	}
	const char* levelResource = "World1.xml";
	m_pGame->VChangeState(BaseEngineState::BGS_LoadingGameEnvironment);
	result = m_pGame->VLoadGame(levelResource);
	if (!result) {
		return false;
	}
	m_pGame->VChangeState(BaseEngineState::BGS_Running);

	return true;
}

void SystemClass::Shutdown() {

	ImGui::DestroyContext();

	// Release the graphics object.
	m_Graphics.Shutdown();

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run() {
	MSG msg;
	bool result;

	m_Timer.Reset();

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	bool done = false;
	while (!done) {
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			// Otherwise do the frame processing.
			m_Timer.Tick();

			Update(m_Timer);
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}

	return;
}

void SystemClass::OnResize() {

	m_Graphics.OnResize(m_engineOptions);
}

Renderer SystemClass::GetRendererType() {
	return m_applicationHandle->m_engineOptions.m_Renderer;
}

GraphicsClass& SystemClass::GetGraphics() {
	return m_applicationHandle->m_Graphics;
}

EngineOptions& SystemClass::GetEngineOptions() {
	return m_applicationHandle->m_engineOptions;
}

BaseEngineLogic* SystemClass::GetEngineLogic() {
	return m_applicationHandle->m_pGame.get();
}

void SystemClass::Update(const GameTimer& gt) {
	m_pGame->VOnUpdate(gt.TotalTime(), gt.DeltaTime());
	IEventManager::Get()->VUpdate(20);
	m_Graphics.Update(m_Input, m_Mouse, gt);
}

bool SystemClass::Frame() {
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input.IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics.Frame();
	if (!result) {
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam)) {
		return true;
	}

	switch (umsg) {

		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it
		// when it becomes active.  
		case WM_ACTIVATE: {
			if (LOWORD(wparam) == WA_INACTIVE) {
				m_AppPaused = true;
				m_Timer.Stop();
			}
			else {
				m_AppPaused = false;
				m_Timer.Start();
			}
			return 0;
		}

		// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE: {
			// Save the new client area dimensions.
			m_engineOptions.m_screenWidth = LOWORD(lparam);
			m_engineOptions.m_screenHeight = HIWORD(lparam);
			if (m_Graphics.GetD3D() && m_Graphics.GetD3D()->GetDevice()) {
				if (wparam == SIZE_MINIMIZED) {
					m_AppPaused = true;
					m_Minimized = true;
					m_Maximized = false;
				}
				else if (wparam == SIZE_MAXIMIZED) {
					m_AppPaused = false;
					m_Minimized = false;
					m_Maximized = true;
					OnResize();
				}
				else if (wparam == SIZE_RESTORED) {

					// Restoring from minimized state?
					if (m_Minimized) {
						m_AppPaused = false;
						m_Minimized = false;
						OnResize();
					}

					// Restoring from maximized state?
					else if (m_Maximized) {
						m_AppPaused = false;
						m_Maximized = false;
						OnResize();
					}
					else if (m_Resizing) {
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						OnResize();
					}
				}
			}
			return 0;
		}

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE: {
			m_AppPaused = true;
			m_Resizing = true;
			m_Timer.Stop();
			return 0;
		}

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE: {
			m_AppPaused = false;
			m_Resizing = false;
			m_Timer.Start();
			OnResize();
			return 0;
		}

		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN: {
			// If a key is pressed send it to the input object so it can record that state.
			m_Input.KeyDown((unsigned int)wparam);
			return 0;
		}

		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR: {
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);
		}

		// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO: {
			((MINMAXINFO*)lparam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lparam)->ptMinTrackSize.y = 200;
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP: {
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input.KeyUp((unsigned int)wparam);
			return 0;
		}

		// ************ MOUSE MESSAGES ************ //
		case WM_MOUSEMOVE: {
			POINTS pt = MAKEPOINTS(lparam);
			if (pt.x > 0 && pt.x < m_engineOptions.m_screenWidth && pt.y > 0 && pt.y < m_engineOptions.m_screenHeight) {
				m_Mouse.OnMouseMove(pt.x, pt.y);
				if (!m_Mouse.IsInWindow()) {
					SetCapture(hwnd);
					m_Mouse.OnMouseEnter();
				}
			}
			else {
				if (wparam & (MK_LBUTTON | MK_RBUTTON)) {
					pt.x = (std::max)(short(0), pt.x);
					pt.x = (std::min)(short(m_engineOptions.m_screenWidth - 1), pt.x);
					pt.y = (std::max)(short(0), pt.y);
					pt.y = (std::min)(short(m_engineOptions.m_screenHeight - 1), pt.y);
					m_Mouse.OnMouseMove(pt.x, pt.y);
				}
				else {
					ReleaseCapture();
					m_Mouse.OnMouseLeave();
					m_Mouse.OnLeftReleased(pt.x, pt.y);
					m_Mouse.OnRightReleased(pt.x, pt.y);
				}
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			const POINTS pt = MAKEPOINTS(lparam);
			m_Mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN: {
			const POINTS pt = MAKEPOINTS(lparam);
			m_Mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP: {
			const POINTS pt = MAKEPOINTS(lparam);
			m_Mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP: {
			const POINTS pt = MAKEPOINTS(lparam);
			m_Mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL: {
			const POINTS pt = MAKEPOINTS(lparam);
			if (GET_WHEEL_DELTA_WPARAM(wparam) > 0) {
				m_Mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wparam) < 0) {
				m_Mouse.OnWheelDown(pt.x, pt.y);
			}
			break;
		}
		// ************ END MOUSE MESSAGES ************ //

		// Any other messages send to the default message handler as our application won't make use of them.
		default: {
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool SystemClass::InitializeWindows() {
	
	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	if (!RegisterClassEx(&wc)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Determine the resolution of the clients desktop screen.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX;
	int posY;

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_engineOptions.m_fullScreen) {

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {
		// If windowed then set it to 800x600 resolution.
		screenWidth = m_engineOptions.m_screenWidth;
		screenHeight = m_engineOptions.m_screenHeight;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		//WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, // окно без рамки
		WS_OVERLAPPEDWINDOW,
		posX,
		posY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		m_hinstance,
		NULL
	);

	if (!m_hwnd) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Init ImGui Win32 Impl
	ImGui_ImplWin32_Init(m_hwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);

	return true;
}

void SystemClass::ShutdownWindows() {

	ImGui_ImplWin32_Shutdown();

	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (m_engineOptions.m_fullScreen) {
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	m_applicationHandle = nullptr;

	return;
}

LRESULT CALLBACK SystemClass::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
		// Check if the window is being destroyed.
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		// Check if the window is being closed.
		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}
		// All other messages pass to the message handler in the system class.
		default: {
			return m_applicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

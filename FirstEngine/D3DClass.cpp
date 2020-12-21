#include "D3DClass.h"

#include "memoryUtility.h"

D3DClass::D3DClass() {
	m_factory				= nullptr;

	m_swapChain				= nullptr;
	m_device				= nullptr;
	m_deviceContext			= nullptr;
}

D3DClass::~D3DClass() {}

bool D3DClass::Initialize(const EngineOptions& options, HWND hwnd) {

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Store the vsync setting.
	m_vsync_enabled = !options.m_runFullSpeed;

	// Create a DirectX graphics interface factory.
	ThrowIfFailed(
		CreateDXGIFactory(IID_PPV_ARGS(m_factory.GetAddressOf()))
	);

	// Use the factory to create an adapter for the primary graphics interface (video card).
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	ThrowIfFailed(
		m_factory->EnumAdapters(0, adapter.GetAddressOf())
	);

	// Enumerate the primary adapter output (monitor).
	Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;
	ThrowIfFailed(
		adapter->EnumOutputs(0, adapterOutput.GetAddressOf())
	);

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	unsigned int numModes;
	ThrowIfFailed(
		adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)
	);

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	std::vector<DXGI_MODE_DESC> displayModeList(numModes);

	// Now fill the display mode list structures.
	ThrowIfFailed(
		adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data())
	);

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	unsigned int numerator = 0;
	unsigned int denominator = 1;
	for (unsigned int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)options.m_screenWidth) {
			if (displayModeList[i].Height == (unsigned int)options.m_screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	ThrowIfFailed(
		adapter->GetDesc(&adapterDesc)
	);

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	unsigned long long stringLength;
	int error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	// Initialize the swap chain description.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = options.m_screenWidth;
	swapChainDesc.BufferDesc.Height = options.m_screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (options.m_fullScreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			createDeviceFlags,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_swapChain.GetAddressOf(),
			m_device.GetAddressOf(),
			NULL,
			m_deviceContext.GetAddressOf()
		)
	);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	return true;
}

void D3DClass::Shutdown() {
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain) {
		m_swapChain->SetFullscreenState(false, NULL);
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha) {

	return;
}

void D3DClass::EndScene() {

	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled) {
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else {
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

void D3DClass::OnResize(const EngineOptions& options) {

	HRESULT result;
}

ID3D11Device* D3DClass::GetDevice() {
	return m_device.Get();
}


ID3D11DeviceContext* D3DClass::GetDeviceContext() {
	return m_deviceContext.Get();
}

IDXGISwapChain* D3DClass::GetSwapChain() {
	return m_swapChain.Get();
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}
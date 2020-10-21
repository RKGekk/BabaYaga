#include "D3DClass.h"

#include "memoryUtility.h"

D3DClass::D3DClass() {
	m_factory				= nullptr;

	m_swapChain				= nullptr;
	m_device				= nullptr;
	m_deviceContext			= nullptr;
	m_renderTargetView		= nullptr;
	m_depthStencilBuffer	= nullptr;
	m_depthStencilState		= nullptr;
	m_depthStencilView		= nullptr;
	m_rasterState			= nullptr;
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

	// Get the pointer to the back buffer.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
	ThrowIfFailed(
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()))
	);

	// Create the render target view with the back buffer pointer.
	ThrowIfFailed(
		m_device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_renderTargetView.GetAddressOf())
	);

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width				= options.m_screenWidth;
	depthBufferDesc.Height				= options.m_screenHeight;
	depthBufferDesc.MipLevels			= 1;
	depthBufferDesc.ArraySize			= 1;
	depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count	= 1;
	depthBufferDesc.SampleDesc.Quality	= 0;
	depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags		= 0;
	depthBufferDesc.MiscFlags			= 0;

	// Create the texture for the depth buffer using the filled out description.
	ThrowIfFailed(
		m_device->CreateTexture2D(&depthBufferDesc, NULL, m_depthStencilBuffer.GetAddressOf())
	);

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable		= true;
	depthStencilDesc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc			= D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable		= true;
	depthStencilDesc.StencilReadMask	= 0xFF;
	depthStencilDesc.StencilWriteMask	= 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	ThrowIfFailed(
		m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf())
	);

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	ThrowIfFailed(
		m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf())
	);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable	= false;
	rasterDesc.CullMode					= D3D11_CULL_BACK;
	rasterDesc.DepthBias				= 0;
	rasterDesc.DepthBiasClamp			= 0.0f;
	rasterDesc.DepthClipEnable			= true;
	rasterDesc.FillMode					= D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise	= false;
	rasterDesc.MultisampleEnable		= false;
	rasterDesc.ScissorEnable			= false;
	rasterDesc.SlopeScaledDepthBias		= 0.0f;

	// Create the rasterizer state from the description we just filled out.
	ThrowIfFailed(
		m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf())
	);

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState.Get());

	// Setup the viewport for rendering.
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)options.m_screenWidth;
	viewport.Height = (float)options.m_screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

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
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	m_renderTargetView->Release();
	m_depthStencilView->Release();
	m_depthStencilBuffer->Release();

	ThrowIfFailed(
		m_swapChain->ResizeBuffers(1, options.m_screenWidth, options.m_screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0)
	);
	
	// Get the pointer to the back buffer.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
	ThrowIfFailed(
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()))
	);

	// Create the render target view with the back buffer pointer.
	ThrowIfFailed(
		m_device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_renderTargetView.GetAddressOf())
	);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = options.m_screenWidth;
	depthStencilDesc.Height = options.m_screenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ThrowIfFailed(
		m_device->CreateTexture2D(&depthStencilDesc, NULL, m_depthStencilBuffer.GetAddressOf())
	);

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf())
	);

	// Bind the render target view and depth/stencil view to the pipeline.
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());


	// Set the viewport transform.
	D3D11_VIEWPORT screenViewport;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = static_cast<float>(options.m_screenWidth);
	screenViewport.Height = static_cast<float>(options.m_screenHeight);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &screenViewport);
}

ID3D11Device* D3DClass::GetDevice() {
	return m_device.Get();
}


ID3D11DeviceContext* D3DClass::GetDeviceContext() {
	return m_deviceContext.Get();
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}
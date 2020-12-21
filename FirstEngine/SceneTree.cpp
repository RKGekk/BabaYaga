#include "SceneTree.h"

#include "RootNode.h"
#include "ISceneNode.h"
#include "IEventData.h"
#include "IEventManager.h"
#include "FastDelegate.h"
#include "memoryUtility.h"
#include "stringUtility.h"
#include "EvtData_Modified_Render_Component.h"
#include "EvtData_New_Render_Component.h"
#include "EvtData_Destroy_Actor.h"
#include "EvtData_Move_Actor.h"
#include "BaseEngineLogic.h"
#include "MatrixStack.h"
#include "Actor.h"
#include "CameraNode.h"
#include "FreeCameraNode.h"
#include "SystemClass.h"
#include "LightManager.h"
#include "memoryUtility.h"
#include "GeometryGenerator.h"
#include "D3D11Offscreen.h"
#include "ShaderResource.h"

SceneTree::SceneTree(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	m_Root.reset(new RootNode());
	m_Offscreen.reset(new D3D11Offscreen(nullptr, device));
	m_MatrixStack = std::shared_ptr<MatrixStack>(new MatrixStack());
	m_LightManager = std::shared_ptr<LightManager>(new LightManager());

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
	pEventMgr->VAddListener(fastdelegate::MakeDelegate(this, &SceneTree::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);

	// off screen teture to render
	D3D11_TEXTURE2D_DESC offscreenTexDesc;
	offscreenTexDesc.Width = options.m_screenWidth;
	offscreenTexDesc.Height = options.m_screenHeight;
	offscreenTexDesc.MipLevels = 1;
	offscreenTexDesc.ArraySize = 1;
	offscreenTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	offscreenTexDesc.SampleDesc.Count = 1;
	offscreenTexDesc.SampleDesc.Quality = 0;
	offscreenTexDesc.Usage = D3D11_USAGE_DEFAULT;
	offscreenTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	offscreenTexDesc.CPUAccessFlags = 0;
	offscreenTexDesc.MiscFlags = 0;

	ThrowIfFailed(
		device->CreateTexture2D(&offscreenTexDesc, NULL, m_offscreenTex.GetAddressOf())
	);

	ThrowIfFailed(device->CreateShaderResourceView(m_offscreenTex.Get(), NULL, m_offscreenShaderResourceView.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(m_offscreenTex.Get(), NULL, m_offscreenRenderTargetView.GetAddressOf()));
	ThrowIfFailed(device->CreateUnorderedAccessView(m_offscreenTex.Get(), NULL, m_offscreenUnorderedAccessView.GetAddressOf()));

	// Get the pointer to the back buffer.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
	ThrowIfFailed(
		SystemClass::GetGraphics().GetD3D()->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()))
	);

	// Create the render target view with the back buffer pointer.
	ThrowIfFailed(
		device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_renderTargetView.GetAddressOf())
	);

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = options.m_screenWidth;
	depthBufferDesc.Height = options.m_screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ThrowIfFailed(
		device->CreateTexture2D(&depthBufferDesc, NULL, m_depthStencilBuffer.GetAddressOf())
	);

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	ThrowIfFailed(
		device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf())
	);

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	ThrowIfFailed(
		device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf())
	);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	//deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	deviceContext->OMSetRenderTargets(1, m_offscreenRenderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	ThrowIfFailed(
		device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf())
	);

	// Now set the rasterizer state.
	deviceContext->RSSetState(m_rasterState.Get());

	// Setup the viewport for rendering.
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)options.m_screenWidth;
	viewport.Height = (float)options.m_screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);
}

SceneTree::~SceneTree() {

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
	pEventMgr->VRemoveListener(fastdelegate::MakeDelegate(this, &SceneTree::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
}

HRESULT SceneTree::OnRender(ID3D11DeviceContext* deviceContext) {
	// The render passes usually go like this 
	// 1. Static objects & terrain
	// 2. Actors (dynamic objects that can move)
	// 3. The Sky
	// 4. Anything with Alpha
	if (m_Root && m_Camera) {
		// The scene root could be anything, but it
		// is usually a SceneNode with the identity
		// matrix

		// Setup the color to clear the buffer to.
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Clear the back buffer.
		deviceContext->ClearRenderTargetView(m_offscreenRenderTargetView.Get(), color);
		deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		deviceContext->OMSetRenderTargets(1, m_offscreenRenderTargetView.GetAddressOf(), m_depthStencilView.Get());

		m_Camera->VRender(this, deviceContext);
		m_LightManager->CalcLighting(this);

		if (m_Root->VPreRender(this, deviceContext) == S_OK) {
			m_Root->VRender(this, deviceContext);
			m_Root->VRenderChildren(this, deviceContext);
			m_Root->VPostRender(this);
		}

		deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
		deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//deviceContext->PSSetShaderResources(0, 1, m_offscreenShaderResourceView.GetAddressOf());
		//deviceContext->PSSetShaderResources(0, 1, &m_offscreenShaderResourceView);
		//deviceContext->PSSetShaderResources(m_startSlot, 1, &srv);
		deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		deviceContext->PSSetShaderResources(0, 1, m_offscreenShaderResourceView.GetAddressOf());
		//
		if (m_Offscreen->VPreRender(this, deviceContext) == S_OK) {
			m_Offscreen->VRender(this, deviceContext);
			m_Offscreen->VRenderChildren(this, deviceContext);
			m_Offscreen->VPostRender(this);
		}
		
		
	}

	return S_OK;
}

HRESULT SceneTree::OnRestore(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	HRESULT result;

	m_renderTargetView->Release();
	m_depthStencilView->Release();
	m_depthStencilBuffer->Release();

	m_offscreenShaderResourceView->Release();
	m_offscreenRenderTargetView->Release();
	m_offscreenUnorderedAccessView->Release();

	// off screen teture to render
	D3D11_TEXTURE2D_DESC offscreenTexDesc;
	offscreenTexDesc.Width = options.m_screenWidth;
	offscreenTexDesc.Height = options.m_screenHeight;
	offscreenTexDesc.MipLevels = 1;
	offscreenTexDesc.ArraySize = 1;
	offscreenTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	offscreenTexDesc.SampleDesc.Count = 1;
	offscreenTexDesc.SampleDesc.Quality = 0;
	offscreenTexDesc.Usage = D3D11_USAGE_DEFAULT;
	offscreenTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	offscreenTexDesc.CPUAccessFlags = 0;
	offscreenTexDesc.MiscFlags = 0;

	ThrowIfFailed(
		device->CreateTexture2D(&offscreenTexDesc, NULL, m_offscreenTex.GetAddressOf())
	);

	ThrowIfFailed(device->CreateShaderResourceView(m_offscreenTex.Get(), NULL, m_offscreenShaderResourceView.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(m_offscreenTex.Get(), NULL, m_offscreenRenderTargetView.GetAddressOf()));
	ThrowIfFailed(device->CreateUnorderedAccessView(m_offscreenTex.Get(), NULL, m_offscreenUnorderedAccessView.GetAddressOf()));

	ThrowIfFailed(
		SystemClass::GetGraphics().GetD3D()->GetSwapChain()->ResizeBuffers(1, options.m_screenWidth, options.m_screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0)
	);

	// Get the pointer to the back buffer.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
	ThrowIfFailed(
		SystemClass::GetGraphics().GetD3D()->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()))
	);

	// Create the render target view with the back buffer pointer.
	ThrowIfFailed(
		device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_renderTargetView.GetAddressOf())
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
		device->CreateTexture2D(&depthStencilDesc, NULL, m_depthStencilBuffer.GetAddressOf())
	);

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf())
	);

	// Bind the render target view and depth/stencil view to the pipeline.
	//deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	deviceContext->OMSetRenderTargets(1, m_offscreenRenderTargetView.GetAddressOf(), m_depthStencilView.Get());


	// Set the viewport transform.
	D3D11_VIEWPORT screenViewport;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = static_cast<float>(options.m_screenWidth);
	screenViewport.Height = static_cast<float>(options.m_screenHeight);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &screenViewport);

	return S_OK;
}


HRESULT SceneTree::OnUpdate(const float deltaMilliseconds, ID3D11DeviceContext* deviceContext) {
	if (!m_Root) {
		return S_OK;
	}

	DWORD elapsedTime = 10;
	return m_Root->VOnUpdate(this, elapsedTime, deviceContext);
}

std::shared_ptr<ISceneNode> SceneTree::FindSceneNode(unsigned int actorId) {
	auto i = m_ActorMap.find(actorId);
	if (i == m_ActorMap.end()) {
		return std::shared_ptr<ISceneNode>();
	}

	return i->second;
}

std::shared_ptr<Actor> SceneTree::FindActor(unsigned int actorId) {
	return MakeStrongPtr(SystemClass::GetEngineLogic()->VGetActor(actorId));
	//return std::shared_ptr<Actor>();

	//auto i = m_ActorMap.find(actorId);
	//if (i == m_ActorMap.end()) {
	//	return std::shared_ptr<Actor>();
	//}
	//
	//return i->second;
}

bool SceneTree::AddChild(unsigned int actorId, std::shared_ptr<ISceneNode> kid) {
	if (actorId != 0) {
		// This allows us to search for this later based on actor id
		m_ActorMap[actorId] = kid;
	}

	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL && m_LightManager->m_Lights.size() + 1 <= MAXIMUM_LIGHTS_SUPPORTED) {
		m_LightManager->m_Lights.push_back(pLight);
	}

	return m_Root->VAddChild(kid);
}

bool SceneTree::RemoveChild(unsigned int actorId) {
	if (actorId == 0) {
		return false;
	}
	std::shared_ptr<ISceneNode> kid = FindSceneNode(actorId);
	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(kid);
	if (pLight != NULL) {
		m_LightManager->m_Lights.remove(pLight);
	}
	m_ActorMap.erase(actorId);
	return m_Root->VRemoveChild(actorId);
}

void SceneTree::NewRenderComponentDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_New_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Render_Component>(pEventData);
	unsigned int actorId = pCastEventData->GetActorId();
	std::shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());
	AddChild(actorId, pSceneNode);
}

void SceneTree::ModifiedRenderComponentDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Modified_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_Modified_Render_Component>(pEventData);

	unsigned int actorId = pCastEventData->GetActorId();
	if (actorId == 0) {
		return;
	}

	if (SystemClass::GetEngineLogic()->GetEngineState() == BaseEngineState::BGS_LoadingGameEnvironment) {
		return;
	}

	std::shared_ptr<ISceneNode> pSceneNode = FindSceneNode(actorId);
	if (pSceneNode) {
		pSceneNode->VOnRestore(this);
	}
}

void SceneTree::DestroyActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
	RemoveChild(pCastEventData->GetId());
}

void SceneTree::MoveActorDelegate(std::shared_ptr<IEventData> pEventData) {
	std::shared_ptr<EvtData_Move_Actor> pCastEventData = std::static_pointer_cast<EvtData_Move_Actor>(pEventData);

	unsigned int actorId = pCastEventData->GetId();
	DirectX::XMFLOAT4X4 transform = pCastEventData->GetMatrix();

	std::shared_ptr<ISceneNode> pNode = FindSceneNode(actorId);
	if (pNode) {
		pNode->VSetTransform(&transform);
	}
}

void SceneTree::SetCamera(std::shared_ptr<FreeCameraNode> camera) {
	m_Camera = camera;
}

const std::shared_ptr<FreeCameraNode> SceneTree::GetCamera() const {
	return m_Camera;
}

void SceneTree::PushAndSetMatrix(const DirectX::XMFLOAT4X4& toWorld) {
	m_MatrixStack->Push();
	m_MatrixStack->MultMatrixLocal(toWorld);
}

void SceneTree::PopMatrix() {
	m_MatrixStack->Pop();
}

const DirectX::XMFLOAT4X4 SceneTree::GetTopMatrix() {
	return m_MatrixStack->GetTop();
}

const DirectX::XMFLOAT4X4 SceneTree::GetTopMatrixT() {
	return m_MatrixStack->GetTopT();
}

std::shared_ptr<LightManager> SceneTree::GetLightManager() {
	return m_LightManager;
}
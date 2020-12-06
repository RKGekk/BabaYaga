#include "D3DLightNode11.h"

#include "LightRenderComponent.h"

D3DLightNode11::D3DLightNode11(BaseRenderComponent* renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : LightNode(renderComponent, props, t) {}

HRESULT D3DLightNode11::VOnUpdate(SceneTree*, float elapsedMs, ID3D11DeviceContext* deviceContext) {
	// light color can change anytime! Check the BaseRenderComponent!
	//LightRenderComponent* lrc = static_cast<LightRenderComponent*>(m_RenderComponent);
	return S_OK;
}
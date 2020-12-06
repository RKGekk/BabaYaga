#pragma once

#include <DirectXMath.h>

#include "LightNode.h"

class D3DLightNode11 : public LightNode {
public:
	D3DLightNode11(BaseRenderComponent* renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t);

	virtual HRESULT VOnUpdate(SceneTree*, float elapsedMs, ID3D11DeviceContext* deviceContext) override;
};
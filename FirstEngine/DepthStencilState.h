#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class DepthStencilState : public Bindable {
public:
	DepthStencilState(ID3D11Device* device);
	DepthStencilState(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc);
	void Bind(ID3D11DeviceContext* deviceContext) override;

protected:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depthStencilState;
};
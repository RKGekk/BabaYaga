#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class RasterizerState : public Bindable {
public:
	RasterizerState(ID3D11Device* device);
	RasterizerState(ID3D11Device* device, const D3D11_RASTERIZER_DESC& rasterDesc);
	void Bind(ID3D11DeviceContext* deviceContext) override;

protected:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterState;
};
#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class BlendState : public Bindable {
public:
	BlendState(ID3D11Device* device);
	BlendState(ID3D11Device* device, const D3D11_BLEND_DESC& blendDesc);
	void Bind(ID3D11DeviceContext* deviceContext) override;

protected:
	Microsoft::WRL::ComPtr<ID3D11BlendState>	m_blendState;
};
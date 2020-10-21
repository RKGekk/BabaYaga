#include "BlendState.h"

BlendState::BlendState(ID3D11Device* device) {

	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = false;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ThrowIfFailed(device->CreateBlendState(&alphaToCoverageDesc, &m_blendState));
}

BlendState::BlendState(ID3D11Device* device, const D3D11_BLEND_DESC& blendDesc) {

	ThrowIfFailed(
		device->CreateBlendState(&blendDesc, m_blendState.GetAddressOf())
	);
}

void BlendState::Bind(ID3D11DeviceContext* deviceContext) {
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	deviceContext->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);
}

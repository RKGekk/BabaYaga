#include "RasterizerState.h"

RasterizerState::RasterizerState(ID3D11Device* device) {

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
		device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf())
	);
}

RasterizerState::RasterizerState(ID3D11Device* device, const D3D11_RASTERIZER_DESC& rasterDesc) {

	ThrowIfFailed(
		device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf())
	);
}

void RasterizerState::Bind(ID3D11DeviceContext* deviceContext) {
	deviceContext->RSSetState(m_rasterState.Get());
}

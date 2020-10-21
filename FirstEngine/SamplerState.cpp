#include "SamplerState.h"

SamplerState::SamplerState(ID3D11Device* device) {

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	ThrowIfFailed(
		device->CreateSamplerState(&samplerDesc, &m_sampleState)
	)
}

SamplerState::SamplerState(ID3D11Device* device, const 	D3D11_SAMPLER_DESC& samplerDesc) {

	// Create the texture sampler state.
	ThrowIfFailed(
		device->CreateSamplerState(&samplerDesc, &m_sampleState)
	)
}

void SamplerState::Bind(ID3D11DeviceContext* deviceContext) {
	deviceContext->PSSetSamplers(0, 1, m_sampleState.GetAddressOf());
}

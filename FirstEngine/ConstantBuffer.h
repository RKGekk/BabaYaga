#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "cbPerFrame.h"
#include "memoryUtility.h"


template <typename T>
class ConstantBuffer : public Bindable {
public:


	ConstantBuffer(ID3D11Device* device, const T& consts) {
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		ThrowIfFailed(
			device->CreateBuffer(&cbd, &csd, &pConstantBuffer)
		);
	}

	ConstantBuffer(ID3D11Device* device) {
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cbPerFrame);
		cbd.StructureByteStride = 0u;
		ThrowIfFailed(
			device->CreateBuffer(&cbd, nullptr, &pConstantBuffer)
		);
	}

	void Update(ID3D11DeviceContext* deviceContext, const T& consts) {
		D3D11_MAPPED_SUBRESOURCE msr;

		ThrowIfFailed(
			deviceContext->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr)
		);
		memcpy(msr.pData, &consts, sizeof(consts));
		deviceContext->Unmap(pConstantBuffer.Get(), 0u);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};
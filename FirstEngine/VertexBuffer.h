#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <vector>

#include "Bindable.h"
#include "memoryUtility.h"

class VertexBuffer : public Bindable {
public:

	template<class V>
	VertexBuffer(ID3D11Device* device, const std::vector<V>& vertices) : stride(sizeof(V)) {

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		bd.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();
		ThrowIfFailed(device->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}
	void Bind(ID3D11DeviceContext* deviceContext) override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

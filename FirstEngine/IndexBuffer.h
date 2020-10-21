#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class IndexBuffer : public Bindable {
public:
	IndexBuffer(ID3D11Device* device, const std::vector<unsigned short>& indices);

	void Bind(ID3D11DeviceContext* deviceContext) override;
	UINT GetCount() const;

protected:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};
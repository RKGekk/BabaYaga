#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class Topology : public Bindable {
public:
	Topology(ID3D11Device* device, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(ID3D11DeviceContext* deviceContext) override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type;
};
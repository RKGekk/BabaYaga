#pragma once

#include <d3d11.h>
#include <wrl.h>

class Bindable {
public:
	virtual void Bind(ID3D11DeviceContext* deviceContext) = 0;
	virtual ~Bindable() = default;
};
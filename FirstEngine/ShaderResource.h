#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Bindable.h"
#include "memoryUtility.h"

class ShaderResource : public Bindable {
public:
	ShaderResource(ID3D11Device* device, ID3D11ShaderResourceView* srv);
	ShaderResource(ID3D11Device* device, ID3D11ShaderResourceView* srv, unsigned int startSlot);
	void Bind(ID3D11DeviceContext* deviceContext) override;
protected:
	ID3D11ShaderResourceView* srv;
	unsigned int m_startSlot;
};
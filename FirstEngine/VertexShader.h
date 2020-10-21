#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include <fstream>

#include "Bindable.h"
#include "memoryUtility.h"

class VertexShader : public Bindable {
public:
	VertexShader(ID3D11Device* device, ID3DBlob* pBytecodeBlob,	ID3D11VertexShader* pVertexShader);
	void Bind(ID3D11DeviceContext* deviceContext) override;
	ID3DBlob* GetBytecode() const;

protected:
	ID3DBlob*			m_pBytecodeBlob;
	ID3D11VertexShader*	m_pVertexShader;
};
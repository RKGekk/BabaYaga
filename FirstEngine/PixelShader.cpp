#include "PixelShader.h"

PixelShader::PixelShader(ID3D11Device* device, ID3DBlob* pBytecodeBlob, ID3D11PixelShader* pPixelShader) {
	m_pBytecodeBlob = pBytecodeBlob;
	m_pPixelShader = pPixelShader;
}

void PixelShader::Bind(ID3D11DeviceContext* deviceContext) {

	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0u);
}

ID3DBlob* PixelShader::GetBytecode() const {
	return m_pBytecodeBlob;
}
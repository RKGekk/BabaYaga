#include "ShaderResource.h"

ShaderResource::ShaderResource(ID3D11Device* device, ID3D11ShaderResourceView* srv) : srv(srv), m_startSlot(0) {}

ShaderResource::ShaderResource(ID3D11Device* device, ID3D11ShaderResourceView* srv, unsigned int startSlot) : srv(srv), m_startSlot(startSlot) {}

void ShaderResource::Bind(ID3D11DeviceContext* deviceContext) {
	deviceContext->PSSetShaderResources(m_startSlot, 1, &srv);
}
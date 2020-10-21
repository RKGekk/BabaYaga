#include "Topology.h"

Topology::Topology(ID3D11Device* device, D3D11_PRIMITIVE_TOPOLOGY type) : type(type) {}

void Topology::Bind(ID3D11DeviceContext* deviceContext) {
	deviceContext->IASetPrimitiveTopology(type);
}
#include "Drawable.h"

Drawable::Drawable() {
	m_worldMatrix = nullptr;
}

Drawable::Drawable(DirectX::XMFLOAT4X4* world) : m_worldMatrix(world) {}

void Drawable::Draw(ID3D11DeviceContext* deviceContext) const {
	for (auto& b : binds) {
		b->Bind(deviceContext);
	}

	deviceContext->DrawIndexed(pIndexBuffer->GetCount(), 0u, 0u);
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) {
	
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) {
	
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
#include "D3D11Drawable.h"

D3D11Drawable::D3D11Drawable(BaseRenderComponent* renderComponent, DirectX::XMFLOAT4X4* pMatrix) : SceneNode (renderComponent, RenderPass::RenderPass_Static, pMatrix) {}

D3D11Drawable::~D3D11Drawable() {}

void D3D11Drawable::AddBind(std::unique_ptr<Bindable> bind) {
	binds.push_back(std::move(bind));
}

void D3D11Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) {
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}

HRESULT D3D11Drawable::VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	for (auto& b : binds) {
		b->Bind(deviceContext);
	}

	deviceContext->DrawIndexed(pIndexBuffer->GetCount(), 0u, 0u);

	return S_OK;
}

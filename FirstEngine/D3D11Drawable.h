#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "SceneNode.h"
#include "Bindable.h"
#include "IndexBuffer.h"

class D3D11Drawable : public SceneNode {
protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf);

	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;

public:

	D3D11Drawable(BaseRenderComponent* renderComponent, DirectX::XMFLOAT4X4* pMatrix);
	virtual ~D3D11Drawable();
	virtual HRESULT VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext);
};
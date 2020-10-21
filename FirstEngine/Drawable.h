#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>

#include "IndexBuffer.h"

class Bindable;

class Drawable {
	
public:
	Drawable();
	Drawable(DirectX::XMFLOAT4X4* world);
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(ID3D11DeviceContext* deviceContext) const;
	virtual void Update(ID3D11DeviceContext* deviceContext, float dt, float tt) = 0;

protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf);

	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;

	DirectX::XMFLOAT4X4* m_worldMatrix;
};
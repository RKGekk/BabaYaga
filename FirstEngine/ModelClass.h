#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>

#include "textureclass.h"
#include "GameTimer.h"

class ModelClass {
private:
	struct VertexType {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&) = delete;
	ModelClass& operator=(const ModelClass&) = delete;
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, TextureClass&);
	void Update(const GameTimer&);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;

	TextureClass*		m_Texture;
	DirectX::XMMATRIX	m_worldMatrix;
};
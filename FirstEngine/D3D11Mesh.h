#pragma once

#include <memory>
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "SceneNode.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "D3D11Drawable.h"

#include "TextureClass.h"
#include "cbPerObject.h"
#include "cbPerFrame.h"

#include "Vertex.h"
#include "IndexedTriangleList.h"
#include "TextureHolder.h"
#include "ShaderClass.h"
#include "ShaderHolder.h"

#include "PixelConstantBuffer.h"
#include "VertexConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "ShaderResource.h"
#include "CameraClass.h"
#include "DDSTextureLoader.h"
#include "GeometryGenerator.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "BlendState.h"

#include "BaseRenderComponent.h"
#include "MeshRenderComponent.h"

#include "stringUtility.h"

class D3D11Mesh : public D3D11Drawable {

private:
	VertexConstantBuffer<cbPerObject>*	m_cbvs;
	PixelConstantBuffer<cbPerObject>*	m_cbps0;
	PixelConstantBuffer<cbPerFrame>*	m_cbps1;

public:

	D3D11Mesh(BaseRenderComponent* renderComponent, DirectX::XMFLOAT4X4* pMatrix, ID3D11Device* device);
	virtual ~D3D11Mesh();
	virtual HRESULT VOnUpdate(SceneTree* pScene, float const elapsedMs, ID3D11DeviceContext* deviceContext);
};
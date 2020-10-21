#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>

#include "memoryUtility.h"

#include "Drawable.h"
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

class MoonCrate : public Drawable {
public:
	MoonCrate(ID3D11Device* device, CameraClass* cam, DirectX::XMFLOAT4X4* world);
	void Update(ID3D11DeviceContext* deviceContext, float dt, float tt) override;

private:

	VertexConstantBuffer<cbPerObject>*	m_cbvs;
	PixelConstantBuffer<cbPerObject>*	m_cbps0;
	PixelConstantBuffer<cbPerFrame>*	m_cbps1;
	CameraClass*						m_camera;
};
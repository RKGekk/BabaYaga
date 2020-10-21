#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>

#include "memoryUtility.h"

#include "Drawable.h"
#include "TextureClass.h"
#include "ShaderClass.h"
#include "ShaderHolder.h"
#include "cbPerObject.h"

#include "Vertex.h"
#include "IndexedTriangleList.h"

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

class Sky : public Drawable {
public:
	Sky(ID3D11Device* device, const std::wstring& texture, CameraClass* cam);
	void Update(ID3D11DeviceContext* deviceContext, float dt, float tt) override;

private:

	VertexConstantBuffer<cbPerObject>*	m_cbvs;
	CameraClass*						m_camera;

	Microsoft::WRL::ComPtr<ID3D11Resource>				m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_textureView;
};
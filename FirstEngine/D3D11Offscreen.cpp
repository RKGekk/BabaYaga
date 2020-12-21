#include "D3D11Offscreen.h"
#include "FreeCameraNode.h"
#include "SceneTree.h"
#include "memoryUtility.h"
#include "LightManager.h"
#include "MeshComponent.h"

D3D11Offscreen::D3D11Offscreen(DirectX::XMFLOAT4X4* pMatrix, ID3D11Device* device) : D3D11Drawable(nullptr, pMatrix) {

	GeometryGenerator::MeshData quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	IndexedTriangleList vertices;
	vertices.vertices.reserve(quad.Vertices.size());
	for (UINT i = 0; i < quad.Vertices.size(); ++i) {
		vertices.vertices.push_back(
			Vertex(
				DirectX::XMFLOAT4(quad.Vertices[i].Position.x, quad.Vertices[i].Position.y, quad.Vertices[i].Position.z, 1.0f),
				DirectX::XMFLOAT3(quad.Vertices[i].Normal.x, quad.Vertices[i].Normal.y, quad.Vertices[i].Normal.z),
				DirectX::XMFLOAT2(quad.Vertices[i].TexC.x, quad.Vertices[i].TexC.y),
				DirectX::XMFLOAT3(quad.Vertices[i].TangentU.x, quad.Vertices[i].TangentU.y, quad.Vertices[i].TangentU.z)
			)
		);
	}
	vertices.indices.reserve(quad.Indices.size());
	for (UINT i = 0; i < quad.Indices.size(); ++i) {
		vertices.indices.push_back(quad.Indices[i]);
	}

	std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>(device, vertices.vertices);
	AddBind(std::move(vb));

	std::unique_ptr<IndexBuffer> ibuf = std::make_unique<IndexBuffer>(device, vertices.indices);
	AddIndexBuffer(std::move(ibuf));

	ShaderClass& vs = ShaderHolder::GetShader(L"PostVS.fx");
	auto pvs = std::make_unique<VertexShader>(device, vs.GetBytecode(), vs.GetVertexShader());
	ID3DBlob* pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	ShaderClass& ps = ShaderHolder::GetShader(L"PostPS.fx");
	auto pps = std::make_unique<PixelShader>(device, ps.GetBytecode(), ps.GetPixelShader());
	ID3DBlob* ppsbc = pps->GetBytecode();
	AddBind(std::move(pps));

	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>(device, pvsbc);
	AddBind(std::move(inputLayout));

	std::unique_ptr<Topology> topology = std::make_unique<Topology>(device, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AddBind(std::move(topology));

	std::unique_ptr<SamplerState> samplerState = std::make_unique<SamplerState>(device);
	AddBind(std::move(samplerState));

	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>(device);
	AddBind(std::move(depthStencilState));

	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>(device);
	AddBind(std::move(rasterizerState));

	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>(device);
	AddBind(std::move(blendState));

	cbPerObject mt;
	DirectX::XMStoreFloat4x4(&mt.worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&mt.viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&mt.projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&mt.worldInvTranspose, DirectX::XMMatrixIdentity());

	std::unique_ptr<VertexConstantBuffer<cbPerObject>> cbvs = std::make_unique<VertexConstantBuffer<cbPerObject>>(device, mt);
	m_cbvs = cbvs.get();
	AddBind(std::move(cbvs));

	std::unique_ptr<PixelConstantBuffer<cbPerObject>> cbps0 = std::make_unique<PixelConstantBuffer<cbPerObject>>(device, mt);
	m_cbps0 = cbps0.get();
	AddBind(std::move(cbps0));
}

D3D11Offscreen::~D3D11Offscreen() {}

HRESULT D3D11Offscreen::VOnUpdate(SceneTree* pScene, float const elapsedMs, ID3D11DeviceContext* deviceContext) {

	return S_OK;
}

HRESULT D3D11Offscreen::VPreRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	SceneNode::VPreRender(pScene, deviceContext);

	return S_OK;
}

#include "Sky.h"

Sky::Sky(ID3D11Device* device, const std::wstring& texture, CameraClass* cam) : m_camera(cam) {

	float skySphereRadius = 5000.0f;

	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	std::vector<Vertex> verts(sphere.Vertices.size());
	for (size_t i = 0; i < sphere.Vertices.size(); i++) {
		verts[i].pos = DirectX::XMFLOAT4(sphere.Vertices[i].Position.x, sphere.Vertices[i].Position.y, sphere.Vertices[i].Position.z, 1.0f);
	}

	std::vector<USHORT> indices16;
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

	IndexedTriangleList model(
		std::move(verts),
		std::move(indices16)
	);

	std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>(device, model.vertices);
	AddBind(std::move(vb));

	std::unique_ptr<IndexBuffer> ibuf = std::make_unique<IndexBuffer>(device, model.indices);
	AddIndexBuffer(std::move(ibuf));

	ShaderClass& vs = ShaderHolder::GetShader(L"SkyVertexShader.fx");
	auto pvs = std::make_unique<VertexShader>(device, vs.GetBytecode(), vs.GetVertexShader());
	ID3DBlob* pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	ShaderClass& ps = ShaderHolder::GetShader(L"SkyPixelShader.fx");
	auto pps = std::make_unique<PixelShader>(device, ps.GetBytecode(), ps.GetPixelShader());
	ID3DBlob* ppsbc = pps->GetBytecode();
	AddBind(std::move(pps));

	DirectX::CreateDDSTextureFromFile(device, texture.c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());

	std::unique_ptr<ShaderResource> srv = std::make_unique<ShaderResource>(device, m_textureView.Get());
	AddBind(std::move(srv));

	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>(device, pvsbc);
	AddBind(std::move(inputLayout));

	std::unique_ptr<Topology> topology = std::make_unique<Topology>(device, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AddBind(std::move(topology));

	std::unique_ptr<SamplerState> samplerState = std::make_unique<SamplerState>(device);
	AddBind(std::move(samplerState));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>(device, depthStencilDesc);
	AddBind(std::move(depthStencilState));

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>(device, rasterDesc);
	AddBind(std::move(rasterizerState));

	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>(device);
	AddBind(std::move(blendState));

	cbPerObject mt;
	DirectX::XMStoreFloat4x4(&mt.worldMatrix, DirectX::XMMatrixIdentity());
	mt.viewMatrix = m_camera->GetViewMatrix4x4T();
	mt.projectionMatrix = m_camera->GetProjectionMatix4x4T();

	std::unique_ptr<VertexConstantBuffer<cbPerObject>> cbvs = std::make_unique<VertexConstantBuffer<cbPerObject>>(device, mt);
	m_cbvs = cbvs.get();
	AddBind(std::move(cbvs));
}

void Sky::Update(ID3D11DeviceContext* deviceContext, float dt, float tt) {

	cbPerObject mt;
	DirectX::XMStoreFloat4x4(&mt.worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMFLOAT3 eyePos = m_camera->GetPosition3f();
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	DirectX::XMMATRIX tr = DirectX::XMMatrixTranspose(XMMatrixMultiply(T, m_camera->GetViewMatrix()));
	DirectX::XMStoreFloat4x4(&mt.viewMatrix, tr);
	mt.projectionMatrix = m_camera->GetProjectionMatix4x4T();

	m_cbvs->Update(deviceContext, mt);
}
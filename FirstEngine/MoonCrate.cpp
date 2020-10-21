#include "MoonCrate.h"

MoonCrate::MoonCrate(ID3D11Device* device, CameraClass* cam, DirectX::XMFLOAT4X4* world) : Drawable(world), m_camera(cam) {

	IndexedTriangleList model = IndexedTriangleList::LoadUVNormals("MoonCrate.obj");

	std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>(device, model.vertices);
	AddBind(std::move(vb));

	std::unique_ptr<IndexBuffer> ibuf = std::make_unique<IndexBuffer>(device, model.indices);
	AddIndexBuffer(std::move(ibuf));

	ShaderClass& vs = ShaderHolder::GetShader(L"BasicNormalMapVertexShader.fx");
	auto pvs = std::make_unique<VertexShader>(device, vs.GetBytecode(), vs.GetVertexShader());
	ID3DBlob* pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	ShaderClass& ps = ShaderHolder::GetShader(L"BasicNormalMapPixelShader.fx");
	auto pps = std::make_unique<PixelShader>(device, ps.GetBytecode(), ps.GetPixelShader());
	ID3DBlob* ppsbc = pps->GetBytecode();
	AddBind(std::move(pps));

	std::unique_ptr<ShaderResource> srv0 = std::make_unique<ShaderResource>(device, TextureHolder::GetTexture("D.bmp").GetTexture());
	AddBind(std::move(srv0));

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
	DirectX::XMStoreFloat4x4(&mt.worldMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(m_worldMatrix)));
	mt.viewMatrix = m_camera->GetViewMatrix4x4T();
	mt.projectionMatrix = m_camera->GetProjectionMatix4x4T();
	// Inverse-transpose is just applied to normals.  So zero out 
	// translation row so that it doesn't get into our inverse-transpose
	// calculation--we don't want the inverse-transpose of the translation.
	DirectX::XMMATRIX A = DirectX::XMLoadFloat4x4(m_worldMatrix);
	A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
	DirectX::XMStoreFloat4x4(&mt.worldInvTranspose, DirectX::XMMatrixInverse(&det, A));

	std::unique_ptr<VertexConstantBuffer<cbPerObject>> cbvs = std::make_unique<VertexConstantBuffer<cbPerObject>>(device, mt);
	m_cbvs = cbvs.get();
	AddBind(std::move(cbvs));

	std::unique_ptr<PixelConstantBuffer<cbPerObject>> cbps0 = std::make_unique<PixelConstantBuffer<cbPerObject>>(device, mt);
	m_cbps0 = cbps0.get();
	AddBind(std::move(cbps0));

	cbPerFrame lt;
	DirectionalLight dl;
	dl.Ambient = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dl.Diffuse = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dl.Specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dl.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	dl.Pad = 1.0f;
	lt.dirLight = dl;
	lt.eyePos = m_camera->GetPosition3f();

	std::unique_ptr<PixelConstantBuffer<cbPerFrame>> cbps1 = std::make_unique<PixelConstantBuffer<cbPerFrame>>(device, lt, 1u);
	m_cbps1 = cbps1.get();
	AddBind(std::move(cbps1));
}

void MoonCrate::Update(ID3D11DeviceContext* deviceContext, float dt, float tt) {

	cbPerObject mt;
	DirectX::XMStoreFloat4x4(&mt.worldMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(m_worldMatrix)));
	mt.viewMatrix = m_camera->GetViewMatrix4x4T();
	mt.projectionMatrix = m_camera->GetProjectionMatix4x4T();
	// Inverse-transpose is just applied to normals.  So zero out 
	// translation row so that it doesn't get into our inverse-transpose
	// calculation--we don't want the inverse-transpose of the translation.
	DirectX::XMMATRIX A = DirectX::XMLoadFloat4x4(m_worldMatrix);
	A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
	DirectX::XMStoreFloat4x4(&mt.worldInvTranspose, DirectX::XMMatrixInverse(&det, A));
	m_cbvs->Update(deviceContext, mt);

	cbPerFrame lt;
	DirectionalLight dl;
	dl.Ambient = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dl.Diffuse = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dl.Specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	//DirectX::XMFLOAT3 dir3f = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	//DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&dir3f);
	//DirectX::XMVECTOR rdir = DirectX::XMVector3Transform(dir, DirectX::XMMatrixRotationY(tt));
	//DirectX::XMStoreFloat3(&dir3f, rdir);

	//dl.Direction = dir3f;
	dl.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	lt.dirLight = dl;
	lt.eyePos = m_camera->GetPosition3f();
	m_cbps1->Update(deviceContext, lt);
}
#include "D3D11Mesh.h"
#include "FreeCameraNode.h"
#include "SceneTree.h"
#include "memoryUtility.h"
#include "LightManager.h"
#include "MeshComponent.h"

D3D11Mesh::D3D11Mesh(BaseRenderComponent* renderComponent, DirectX::XMFLOAT4X4* pMatrix, ID3D11Device* device) : D3D11Drawable(renderComponent, pMatrix) {
	MeshRenderComponent* mrc = static_cast<MeshRenderComponent*>(renderComponent);
	std::shared_ptr<MeshComponent> mc = MakeStrongPtr(mrc->GetOwner()->GetComponent<MeshComponent>(MeshComponent::g_Name));

	MeshComponent::IndexedTriangleList model = mc->GetTriangleList();

	std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>(device, model.vertices);
	AddBind(std::move(vb));

	std::unique_ptr<IndexBuffer> ibuf = std::make_unique<IndexBuffer>(device, model.indices);
	AddIndexBuffer(std::move(ibuf));

	ShaderClass& vs = ShaderHolder::GetShader(s2ws(mrc->GetVertexShaderResource()));
	auto pvs = std::make_unique<VertexShader>(device, vs.GetBytecode(), vs.GetVertexShader());
	ID3DBlob* pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	ShaderClass& ps = ShaderHolder::GetShader(s2ws(mrc->GetPixelShaderResource()));
	auto pps = std::make_unique<PixelShader>(device, ps.GetBytecode(), ps.GetPixelShader());
	ID3DBlob* ppsbc = pps->GetBytecode();
	AddBind(std::move(pps));

	std::unique_ptr<ShaderResource> srv0 = std::make_unique<ShaderResource>(device, TextureHolder::GetTexture(mc->GetMaterials()[0].diffuse_texname).GetTexture());
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

	cbPerFrame lt;
	DirectionalLight dl;
	dl.Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dl.Diffuse = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dl.Specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dl.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f);
	dl.Pad = 1.0f;
	lt.dirLight = dl;
	lt.eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	std::unique_ptr<PixelConstantBuffer<cbPerFrame>> cbps1 = std::make_unique<PixelConstantBuffer<cbPerFrame>>(device, lt, 1u);
	m_cbps1 = cbps1.get();
	AddBind(std::move(cbps1));
}

D3D11Mesh::~D3D11Mesh() {}

HRESULT D3D11Mesh::VOnUpdate(SceneTree* pScene, float const elapsedMs, ID3D11DeviceContext* deviceContext) {

	return S_OK;
}

HRESULT D3D11Mesh::VPreRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	SceneNode::VPreRender(pScene, deviceContext);

	const std::shared_ptr<FreeCameraNode> camera = pScene->GetCamera();

	cbPerObject mt;
	mt.worldMatrix = pScene->GetTopMatrixT();
	mt.viewMatrix = camera->GetViewMatrix4x4T();
	mt.projectionMatrix = camera->GetProjectionMatix4x4T();
	// Inverse-transpose is just applied to normals.  So zero out 
	// translation row so that it doesn't get into our inverse-transpose
	// calculation--we don't want the inverse-transpose of the translation.
	DirectX::XMMATRIX A = DirectX::XMLoadFloat4x4(&mt.worldMatrix);
	A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
	DirectX::XMStoreFloat4x4(&mt.worldInvTranspose, DirectX::XMMatrixInverse(&det, A));

	unsigned int componentId = ActorComponent::GetIdFromName("MeshComponent");
	std::shared_ptr<MeshComponent> rc = MakeStrongPtr(m_RenderComponent->GetOwner()->GetComponent<MeshComponent>(componentId));
	const tinyobj::material_t& material = rc->GetMaterials()[0];
	mt.material.Ambient.x = material.ambient[0];
	mt.material.Ambient.y = material.ambient[1];
	mt.material.Ambient.z = material.ambient[2];
	mt.material.Diffuse.x = material.diffuse[0];
	mt.material.Diffuse.y = material.diffuse[1];
	mt.material.Diffuse.z = material.diffuse[2];
	mt.material.Specular.x = material.specular[0];
	mt.material.Specular.y = material.specular[1];
	mt.material.Specular.z = material.specular[2];
	mt.material.Specular.w = material.shininess;
	mt.material.Reflect = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_cbvs->Update(deviceContext, mt);
	m_cbps0->Update(deviceContext, mt);

	std::shared_ptr<LightManager> lightManager = pScene->GetLightManager();
	

	cbPerFrame lt;
	lightManager->CalcLighting(&lt, this);
	//DirectionalLight dl;
	//dl.Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//dl.Diffuse = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	//dl.Specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//
	//dl.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f);
	//lt.dirLight = dl;
	//lt.eyePos = camera->GetPosition3f();
	m_cbps1->Update(deviceContext, lt);

	return S_OK;
}

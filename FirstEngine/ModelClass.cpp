#include "modelclass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;

	m_worldMatrix = DirectX::XMMatrixIdentity();
}

ModelClass::~ModelClass() {}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureClass& texture) {
	bool result;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// Load the texture for this model.
	m_Texture = &texture;

	return true;
}

void ModelClass::Update(const GameTimer& gt) {
	float rps = gt.TotalTime() * DirectX::XM_PI;
	m_worldMatrix = DirectX::XMMatrixRotationY(rps);
}

void ModelClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix) {
	worldMatrix = m_worldMatrix;
	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext) {
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount() {
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture() {
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	std::vector<VertexType> vertices(m_vertexCount);

	// Create the index array.
	std::vector<unsigned long> indices(m_indexCount);

	// Load the vertex array with data.
	vertices[0].position = DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f);	// Top Left
	vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);		// Top Right
	vertices[1].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);	// Bottom Right
	vertices[2].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	vertices[3].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);	// Bottom Left
	vertices[3].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.
	indices[3] = 0;  // Bottom left.
	indices[4] = 2;  // Top middle.
	indices[5] = 3;  // Bottom right.


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.GetAddressOf());
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.GetAddressOf());
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


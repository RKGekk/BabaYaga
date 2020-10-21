#include "FreeCameraNode.h"
#include "EngineOptions.h"
#include "SystemClass.h"

FreeCameraNode::FreeCameraNode() : FreeCameraNode(nullptr) {}

FreeCameraNode::FreeCameraNode(DirectX::XMFLOAT4X4* t) : SceneNode(nullptr, RenderPass::RenderPass_0, t) {
	m_position	= { 0.0f, 0.0f, 0.0f };
	m_Right		= { 1.0f, 0.0f, 0.0f };
	m_Up		= { 0.0f, 1.0f, 0.0f };
	m_Look		= { 0.0f, 0.0f, 1.0f };

	EngineOptions& eo = SystemClass::GetEngineOptions();
	SetLens(eo.m_fov, eo.m_aspectRatio, eo.m_screenNear, eo.m_screenDepth);
}

HRESULT FreeCameraNode::VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	DirectX::XMVECTOR R = DirectX::XMLoadFloat3(&m_Right);
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&m_Up);
	DirectX::XMVECTOR L = DirectX::XMLoadFloat3(&m_Look);
	DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&m_position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = DirectX::XMVector3Normalize(L);
	U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = DirectX::XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
	float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
	float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

	DirectX::XMStoreFloat3(&m_Right, R);
	DirectX::XMStoreFloat3(&m_Up, U);
	DirectX::XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;

	return S_OK;
}

const DirectX::BoundingFrustum& FreeCameraNode::GetFrustum() {
	return m_Frustum;
}

void FreeCameraNode::SetPosition(float x, float y, float z) {
	m_position = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 FreeCameraNode::GetPosition3f() const {
	return m_position;
}

DirectX::XMVECTOR FreeCameraNode::GetPosition() const {
	return DirectX::XMLoadFloat3(&m_position);
}

DirectX::XMVECTOR FreeCameraNode::GetRight()const {
	return DirectX::XMLoadFloat3(&m_Right);
}

DirectX::XMFLOAT3 FreeCameraNode::GetRight3f()const {
	return m_Right;
}

DirectX::XMVECTOR FreeCameraNode::GetUp()const {
	return DirectX::XMLoadFloat3(&m_Up);
}

DirectX::XMFLOAT3 FreeCameraNode::GetUp3f()const {
	return m_Up;
}

DirectX::XMVECTOR FreeCameraNode::GetLook()const {
	return DirectX::XMLoadFloat3(&m_Look);
}

DirectX::XMFLOAT3 FreeCameraNode::GetLook3f()const {
	return m_Look;
}

float FreeCameraNode::GetNearZ()const {
	return m_NearZ;
}

float FreeCameraNode::GetFarZ()const {
	return m_FarZ;
}

float FreeCameraNode::GetAspect()const {
	return m_Aspect;
}

float FreeCameraNode::GetFovY()const {
	return m_FovY;
}

float FreeCameraNode::GetFovX()const {
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float FreeCameraNode::GetNearWindowWidth()const {
	return m_Aspect * m_NearWindowHeight;
}

float FreeCameraNode::GetNearWindowHeight()const {
	return m_NearWindowHeight;
}

float FreeCameraNode::GetFarWindowWidth()const {
	return m_Aspect * m_FarWindowHeight;
}

float FreeCameraNode::GetFarWindowHeight()const {
	return m_FarWindowHeight;
}

void FreeCameraNode::SetLens(float fovY, float aspect, float zn, float zf) {
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	DirectX::XMStoreFloat4x4(&m_Projection, P);
	m_Frustum = DirectX::BoundingFrustum(P);
}

void FreeCameraNode::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp) {
	DirectX::XMVECTOR L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, pos));
	DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
	DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

	DirectX::XMStoreFloat3(&m_position, pos);
	DirectX::XMStoreFloat3(&m_Look, L);
	DirectX::XMStoreFloat3(&m_Right, R);
	DirectX::XMStoreFloat3(&m_Up, U);
}

void FreeCameraNode::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up) {
	DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&up);

	LookAt(P, T, U);
}

DirectX::XMFLOAT4X4 FreeCameraNode::GetViewMatrix4x4() const {
	return m_View;
}

DirectX::XMFLOAT4X4 FreeCameraNode::GetViewMatrix4x4T() const {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_View));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMMATRIX FreeCameraNode::GetViewMatrix()const {
	return DirectX::XMLoadFloat4x4(&m_View);
}

DirectX::XMMATRIX FreeCameraNode::GetProjectionMatix()const {
	return DirectX::XMLoadFloat4x4(&m_Projection);
}

DirectX::XMFLOAT4X4 FreeCameraNode::GetProjectionMatix4x4()const {
	return m_Projection;
}

DirectX::XMFLOAT4X4 FreeCameraNode::GetProjectionMatix4x4T()const {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_Projection));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMMATRIX FreeCameraNode::GetViewProjectionMatrix()const {
	return DirectX::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatix());
}

void FreeCameraNode::Strafe(float d) {
	// mPosition += d*mRight
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&m_Right);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&m_position);
	DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void FreeCameraNode::Walk(float d) {
	// mPosition += d*mLook
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&m_Look);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&m_position);
	DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(s, l, p));
}

void FreeCameraNode::Pitch(float angle) {
	// Rotate up and look vector about the right vector.

	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_Right), angle);

	DirectX::XMStoreFloat3(&m_Up, XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_Up), R));
	DirectX::XMStoreFloat3(&m_Look, XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_Look), R));
}

void FreeCameraNode::RotateY(float angle) {
	// Rotate the basis vectors about the world y-axis.

	DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	DirectX::XMStoreFloat3(&m_Right, XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_Right), R));
	DirectX::XMStoreFloat3(&m_Up, XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_Up), R));
	DirectX::XMStoreFloat3(&m_Look, XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_Look), R));
}

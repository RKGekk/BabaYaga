#include "SceneNodeProperties.h"

SceneNodeProperties::SceneNodeProperties() {
	m_ActorId = 0;
	m_Radius = 0;

	DirectX::XMStoreFloat4x4(&m_ToWorld, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_FromWorld, DirectX::XMMatrixIdentity());
}

unsigned int SceneNodeProperties::ActorId() const {
	return m_ActorId;
}

DirectX::XMFLOAT4X4 const& SceneNodeProperties::ToWorld() const {
	return m_ToWorld;
}

DirectX::XMFLOAT4X4 const& SceneNodeProperties::FromWorld() const {
	return m_FromWorld;
}

void SceneNodeProperties::Transform(DirectX::XMFLOAT4X4* toWorld, DirectX::XMFLOAT4X4* fromWorld) const {
	if (toWorld)
		*toWorld = m_ToWorld;

	if (fromWorld)
		*fromWorld = m_FromWorld;
}

const char* SceneNodeProperties::Name() const {
	return m_Name.c_str();
}

RenderPass SceneNodeProperties::RenderPass() const {
	return m_RenderPass;
}

float SceneNodeProperties::Radius() const {
	return m_Radius;
}
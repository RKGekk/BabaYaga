#pragma once

#include <string>
#include <DirectXMath.h>
#include "RenderPass.h"
#include "Material.h"

class SceneNodeProperties {
	friend class SceneNode;

protected:
	unsigned int		m_ActorId;
	std::string			m_Name;
	DirectX::XMFLOAT4X4	m_ToWorld;
	DirectX::XMFLOAT4X4	m_FromWorld;
	float				m_Radius;
	RenderPass			m_RenderPass;
	Material			m_Material;

	void SetAlpha(const float alpha) {}

public:
	SceneNodeProperties();
	unsigned int ActorId() const;
	DirectX::XMFLOAT4X4 const& ToWorld() const;
	DirectX::XMFLOAT4X4 const& FromWorld() const;
	void Transform(DirectX::XMFLOAT4X4* toWorld, DirectX::XMFLOAT4X4* fromWorld) const;

	const char* Name() const;

	RenderPass RenderPass() const;
	float Radius() const;

	Material GetMaterial() const;
	void SetMaterial(Material mt);
};
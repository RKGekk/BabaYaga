#pragma once

#include <DirectXCollision.h>
#include <memory>

#include "SceneNode.h"
#include "BaseRenderComponent.h"

// A camera node controls the D3D view transform and holds the view
// frustum definition
class CameraNode : public SceneNode {
public:

	CameraNode(DirectX::XMFLOAT4X4* t);

	virtual HRESULT VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) override;
	virtual HRESULT VOnRestore(SceneTree* pScene) override;

	const DirectX::BoundingFrustum& GetFrustum();
	void SetTarget(std::shared_ptr<SceneNode> pTarget);
	void ClearTarget();
	std::shared_ptr<SceneNode> GetTarget();

	DirectX::XMFLOAT4X4 GetWorldViewProjection(SceneTree* pScene);
	HRESULT SetViewTransform(SceneTree* pScene);

	DirectX::XMFLOAT4X4 GetProjection();
	DirectX::XMFLOAT4X4 GetView();

	void SetCameraOffset(const DirectX::XMFLOAT4& cameraOffset);

protected:

	DirectX::BoundingFrustum	m_Frustum;
	DirectX::XMFLOAT4X4			m_Projection;
	DirectX::XMFLOAT4X4			m_View;

	bool						m_bActive;
	bool						m_DebugCamera;

	std::shared_ptr<SceneNode>	m_pTarget;
	DirectX::XMFLOAT4			m_CamOffsetVector;	//Direction of camera relative to target.
};
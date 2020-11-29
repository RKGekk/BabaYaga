#pragma once

#include <memory>
#include <vector>

#include "ISceneNode.h"
#include "SceneNodeProperties.h"
#include "RenderPass.h"

class SceneTree;
class BaseRenderComponent;

using SceneNodeList = std::vector<std::shared_ptr<ISceneNode>>;

class SceneNode : public ISceneNode {
	friend class SceneTree;

protected:
	SceneNodeList			m_Children;
	SceneNode*				m_pParent;
	SceneNodeProperties		m_Props;
	BaseRenderComponent*	m_RenderComponent;

public:
	SceneNode(BaseRenderComponent* renderComponent, RenderPass renderPass, DirectX::XMFLOAT4X4* to = nullptr, DirectX::XMFLOAT4X4* from = nullptr);

	virtual const SceneNodeProperties* const VGet() const;

	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(const DirectX::XMFLOAT3& pos);

	const DirectX::XMFLOAT3 GetWorldPosition() const; // respect ancestor's position
	DirectX::XMFLOAT3 GetDirection() const;

	void SetRadius(const float radius);


	// ***** Implementation pure virtual functions *****
	virtual void VSetTransform(DirectX::XMFLOAT4X4* toWorld, DirectX::XMFLOAT4X4* fromWorld = NULL) override;

	virtual HRESULT VOnUpdate(SceneTree*, float const elapsedMs, ID3D11DeviceContext* deviceContext) override;

	virtual HRESULT VPreRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) override;
	virtual HRESULT VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) override;
	virtual HRESULT VRenderChildren(SceneTree* pScene, ID3D11DeviceContext* deviceContext) override;
	virtual HRESULT VPostRender(SceneTree* pScene) override;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) override;
	virtual bool VRemoveChild(unsigned int id) override;

	virtual ~SceneNode() override;
};
#pragma once

#include <memory>
#include <d3d11.h>

#include "SceneNode.h"

class RootNode : public SceneNode {
public:
	RootNode();
	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid);
	virtual HRESULT VRenderChildren(SceneTree* pScene, ID3D11DeviceContext* deviceContext);
	virtual bool VRemoveChild(unsigned int id);
	virtual bool VIsVisible(SceneTree* pScene) const { return true; }
};
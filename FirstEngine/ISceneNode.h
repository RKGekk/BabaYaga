#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>

class SceneTree;
class SceneNodeProperties;

class ISceneNode {
public:
	virtual const SceneNodeProperties* const VGet() const = 0;

	virtual void VSetTransform(const DirectX::XMFLOAT4X4* toWorld, const  DirectX::XMFLOAT4X4* fromWorld = NULL) = 0;

	virtual HRESULT VOnUpdate(SceneTree* pScene, float elapsedMs, ID3D11DeviceContext* deviceContext) = 0;

	virtual HRESULT VPreRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) = 0;
	virtual HRESULT VRender(SceneTree* pScene, ID3D11DeviceContext* deviceContext) = 0;
	virtual HRESULT VRenderChildren(SceneTree* pScene, ID3D11DeviceContext* deviceContext) = 0;
	virtual HRESULT VPostRender(SceneTree* pScene) = 0;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) = 0;
	virtual bool VRemoveChild(unsigned int id) = 0;

	virtual ~ISceneNode() {};
};
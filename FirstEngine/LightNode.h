#pragma once

#include <DirectXMath.h>


#include "SceneNode.h"
#include "LightProperties.h"

class LightNode : public SceneNode {
protected:
	LightProperties m_LightProps;

public:
	LightNode(BaseRenderComponent* renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t);

	virtual const LightProperties* const VGetLight() const;
	virtual HRESULT VOnRestore(SceneTree* pScene) override;
};
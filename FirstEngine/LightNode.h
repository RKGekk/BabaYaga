#pragma once

#include <DirectXMath.h>

#include "SceneNode.h"
#include "LightProperties.h"

class LightNode : public SceneNode {
protected:
	LightProperties m_LightProps;

public:
	LightNode(const unsigned int actorId, BaseRenderComponent* renderComponent, const LightProperties& props, DirectX::XMFLOAT4X4* t);
};
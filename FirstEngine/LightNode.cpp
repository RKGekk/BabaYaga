#include "LightNode.h"
#include "RenderPass.h"

LightNode::LightNode(BaseRenderComponent* renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(renderComponent, RenderPass::RenderPass_NotRendered, t) {
	m_LightProps = props;
}

const LightProperties* const LightNode::VGetLight() const {
	return &m_LightProps;
}

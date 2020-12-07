#include "LightNode.h"
#include "RenderPass.h"

#include "BaseRenderComponent.h"
#include "LightRenderComponent.h"
#include "memoryUtility.h"

LightNode::LightNode(BaseRenderComponent* renderComponent, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(renderComponent, RenderPass::RenderPass_NotRendered, t) {
	m_LightProps = props;
}

const LightProperties* const LightNode::VGetLight() const {
	return &m_LightProps;
}

HRESULT LightNode::VOnRestore(SceneTree* pScene) {
	unsigned int componentId = ActorComponent::GetIdFromName("LightRenderComponent");
	std::shared_ptr<LightRenderComponent> rc = MakeStrongPtr(m_RenderComponent->GetOwner()->GetComponent<LightRenderComponent>(componentId));
	m_LightProps = rc->GetLight();
	SceneNode::VOnRestore(pScene);
	return S_OK;
}

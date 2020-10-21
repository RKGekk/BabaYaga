#include "RootNode.h"
#include "RenderPass.h"
#include "SceneNodeProperties.h"

RootNode::RootNode() : SceneNode(nullptr, RenderPass::RenderPass_0) {
	m_Children.reserve(static_cast<int>(RenderPass::RenderPass_Last));

	std::shared_ptr<SceneNode> staticGroup(new SceneNode(nullptr, RenderPass::RenderPass_Static));
	m_Children.push_back(staticGroup);	// RenderPass_Static = 0

	std::shared_ptr<SceneNode> actorGroup(new SceneNode(nullptr, RenderPass::RenderPass_Actor));
	m_Children.push_back(actorGroup);	// RenderPass_Actor = 1

	std::shared_ptr<SceneNode> skyGroup(new SceneNode(nullptr, RenderPass::RenderPass_Sky));
	m_Children.push_back(skyGroup);	// RenderPass_Sky = 2

	std::shared_ptr<SceneNode> invisibleGroup(new SceneNode(nullptr, RenderPass::RenderPass_NotRendered));
	m_Children.push_back(invisibleGroup);	// RenderPass_NotRendered = 3
}

bool RootNode::VAddChild(std::shared_ptr<ISceneNode> kid) {
	// The Root node has children that divide the scene graph into render passes.
	// Scene nodes will get added to these children based on the value of the
	// render pass member variable.

	RenderPass pass = kid->VGet()->RenderPass();
	if ((unsigned)pass >= m_Children.size() || !m_Children[static_cast<int>(pass)]) {
		return false;
	}

	return m_Children[static_cast<int>(pass)]->VAddChild(kid);
}

// Returns false if nothing was removed
bool RootNode::VRemoveChild(unsigned int id) {
	bool anythingRemoved = false;
	for (int i = static_cast<int>(RenderPass::RenderPass_0); i < static_cast<int>(RenderPass::RenderPass_Last); ++i) {
		if (m_Children[i]->VRemoveChild(id)) {
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

HRESULT RootNode::VRenderChildren(SceneTree* pScene, ID3D11DeviceContext* deviceContext) {
	// This code creates fine control of the render passes.

	for (int pass = static_cast<int>(RenderPass::RenderPass_0); pass < static_cast<int>(RenderPass::RenderPass_Last); ++pass) {
		switch (pass) {
			case static_cast<int>(RenderPass::RenderPass_Static) : {
				m_Children[pass]->VRenderChildren(pScene, deviceContext);
			}
			break;
			case static_cast<int>(RenderPass::RenderPass_Actor) : {
				m_Children[pass]->VRenderChildren(pScene, deviceContext);
			}
			break;
			case static_cast<int>(RenderPass::RenderPass_Sky) : {
				m_Children[pass]->VRenderChildren(pScene, deviceContext);
			}
			break;
		}
	}

	return S_OK;
}
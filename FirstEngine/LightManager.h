#pragma once

#include <memory>
#include <list>
#include <DirectXMath.h>

#include "SceneTree.h"
#include "LightNode.h"
#include "cbPerFrame.h"

using Lights = std::list<std::shared_ptr<LightNode>>;
#define MAXIMUM_LIGHTS_SUPPORTED (1)

class LightManager {
	friend class SceneTree;

protected:
	Lights m_Lights;
	DirectX::XMFLOAT4 m_vLightDir[MAXIMUM_LIGHTS_SUPPORTED];
	DirectX::XMFLOAT4 m_vLightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
	DirectX::XMFLOAT4 m_vLightAmbient;

public:
	void CalcLighting(SceneTree* pScene);
	void CalcLighting(cbPerFrame* pLighting, SceneNode* pNode);
	int GetLightCount(const SceneNode* node);

	const DirectX::XMFLOAT4* GetLightAmbient(const SceneNode* node);
	const DirectX::XMFLOAT4* GetLightDirection(const SceneNode* node);
	const DirectX::XMFLOAT4* GetLightDiffuse(const SceneNode* node);
};
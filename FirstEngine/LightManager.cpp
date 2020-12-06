#include "LightManager.h"

void LightManager::CalcLighting(SceneTree* pScene) {
	int count = 0;

	for (Lights::iterator i = m_Lights.begin(); i != m_Lights.end(); ++i, ++count) {
		std::shared_ptr<LightNode> light = *i;

		DirectX::XMFLOAT3 lightDir = light->GetDirection();
		m_vLightDir[count] = DirectX::XMFLOAT4(lightDir.x, lightDir.y, lightDir.z, 1.0f);
		m_vLightDiffuse[count] = light->VGetLight()->m_Diffuse;
	}
}

void LightManager::CalcLighting(cbPerFrame* pLighting, SceneNode* pNode) {
	DirectionalLight dl;
	dl.Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dl.Diffuse = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dl.Specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dl.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f);
	dl.Pad = 1.0f;
	pLighting->dirLight = dl;
	pLighting->eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

int LightManager::GetLightCount(const SceneNode* node) {
	return m_Lights.size();
}

const DirectX::XMFLOAT4* LightManager::GetLightAmbient(const SceneNode* node) {
	return &m_vLightAmbient;
}

const DirectX::XMFLOAT4* LightManager::GetLightDirection(const SceneNode* node) {
	return m_vLightDir;
}

const DirectX::XMFLOAT4* LightManager::GetLightDiffuse(const SceneNode* node) {
	return m_vLightDiffuse;
}
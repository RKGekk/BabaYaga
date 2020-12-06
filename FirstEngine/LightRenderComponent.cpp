#include "LightRenderComponent.h"

#include "TransformComponent.h"
#include "memoryUtility.h"
#include "stringUtility.h"
#include "Actor.h"
#include "D3D11Mesh.h"
#include "SystemClass.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "D3DLightNode11.h"

const std::string LightRenderComponent::g_Name = "LightRenderComponent";

LightRenderComponent::LightRenderComponent() {}

const std::string& LightRenderComponent::VGetName() const {
	return g_Name;
}

bool LightRenderComponent::VDelegateInit(TiXmlElement* pData) {
	TiXmlElement* pLight = pData->FirstChildElement("Light");

	double temp;
	TiXmlElement* pAttenuationNode = NULL;
	pAttenuationNode = pLight->FirstChildElement("Attenuation");
	if (pAttenuationNode) {
		double temp;
		pAttenuationNode->Attribute("const", &temp);
		m_Props.m_Attenuation[0] = (float)temp;

		pAttenuationNode->Attribute("linear", &temp);
		m_Props.m_Attenuation[1] = (float)temp;

		pAttenuationNode->Attribute("exp", &temp);
		m_Props.m_Attenuation[2] = (float)temp;
	}

	TiXmlElement* pAmbientNode = NULL;
	pAmbientNode = pLight->FirstChildElement("Ambient");
	if (pAmbientNode) {
		double temp;
		pAmbientNode->Attribute("r", &temp);
		m_Props.m_Ambient.x = (float)temp;

		pAmbientNode->Attribute("g", &temp);
		m_Props.m_Ambient.y = (float)temp;

		pAmbientNode->Attribute("b", &temp);
		m_Props.m_Ambient.z = (float)temp;

		pAmbientNode->Attribute("a", &temp);
		m_Props.m_Ambient.w = (float)temp;
	}

	TiXmlElement* pDiffuseNode = NULL;
	pDiffuseNode = pLight->FirstChildElement("Diffuse");
	if (pDiffuseNode) {
		double temp;
		pDiffuseNode->Attribute("r", &temp);
		m_Props.m_Diffuse.x = (float)temp;

		pDiffuseNode->Attribute("g", &temp);
		m_Props.m_Diffuse.y = (float)temp;

		pDiffuseNode->Attribute("b", &temp);
		m_Props.m_Diffuse.z = (float)temp;

		pDiffuseNode->Attribute("a", &temp);
		m_Props.m_Diffuse.w = (float)temp;
	}

	TiXmlElement* pSpecularNode = NULL;
	pSpecularNode = pLight->FirstChildElement("Specular");
	if (pSpecularNode) {
		double temp;
		pSpecularNode->Attribute("r", &temp);
		m_Props.m_Specular.x = (float)temp;

		pSpecularNode->Attribute("g", &temp);
		m_Props.m_Specular.y = (float)temp;

		pSpecularNode->Attribute("b", &temp);
		m_Props.m_Specular.z = (float)temp;

		pSpecularNode->Attribute("a", &temp);
		m_Props.m_Specular.w = (float)temp;
	}

	TiXmlElement* pShapeNode = NULL;
	pShapeNode = pLight->FirstChildElement("Shape");
	if (pShapeNode) {
		pShapeNode->Attribute("range", &temp);
		m_Props.m_Range = (float)temp;
		pShapeNode->Attribute("spot", &temp);
		m_Props.m_Spot = (float)temp;
	}
	return true;
}

std::shared_ptr<SceneNode> LightRenderComponent::VCreateSceneNode() {
	std::shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	if (pTransformComponent) {
		BaseRenderComponent* weakThis(this);
		return shared_ptr<SceneNode>(new D3DLightNode11(weakThis, m_Props, &(pTransformComponent->GetTransform4x4f())));
	}
	return std::shared_ptr<SceneNode>();
}
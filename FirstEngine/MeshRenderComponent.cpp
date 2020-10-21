#include "MeshRenderComponent.h"

#include "TransformComponent.h"
#include "memoryUtility.h"
#include "Actor.h"
#include "D3D11Mesh.h"
#include "SystemClass.h"
#include "GraphicsClass.h"
#include "D3DClass.h"

const char* MeshRenderComponent::g_Name = "MeshRenderComponent";

MeshRenderComponent::MeshRenderComponent() {
	m_textureResource = "";
	m_objResource = "";
}

const char* MeshRenderComponent::VGetName() const {
	return g_Name;
}

const char* MeshRenderComponent::GetTextureResource() {
	return m_textureResource.c_str();
}

const char* MeshRenderComponent::GetObjResource() {
	return m_objResource.c_str();
}

const char* MeshRenderComponent::GetPixelShaderResource() {
	return m_pixelShaderResource.c_str();
}

const char* MeshRenderComponent::GetVertexShaderResource() {
	return m_vertexShaderResource.c_str();
}

bool MeshRenderComponent::VDelegateInit(TiXmlElement* pData) {

	TiXmlElement* pTexture = pData->FirstChildElement("Texture");
	if (pTexture) {
		m_textureResource = pTexture->FirstChild()->Value();
	}

	TiXmlElement* pObj = pData->FirstChildElement("Obj");
	if (pObj) {
		m_objResource = pObj->FirstChild()->Value();
	}

	TiXmlElement* pPixelShader = pData->FirstChildElement("PixelShader");
	if (pPixelShader) {
		m_pixelShaderResource = pPixelShader->FirstChild()->Value();
	}

	TiXmlElement* pVertexShader = pData->FirstChildElement("VertexShader");
	if (pVertexShader) {
		m_vertexShaderResource = pVertexShader->FirstChild()->Value();
	}

	return true;
}

std::shared_ptr<SceneNode> MeshRenderComponent::VCreateSceneNode() {
	std::shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	if (pTransformComponent) {
		BaseRenderComponent* weakThis(this);
		return std::shared_ptr<SceneNode>(new D3D11Mesh(weakThis, &(pTransformComponent->GetTransform4x4f()), SystemClass::GetGraphics().GetD3D()->GetDevice()));
	}

	return std::shared_ptr<SceneNode>();
}
#include "MeshRenderComponent.h"

#include "TransformComponent.h"
#include "memoryUtility.h"
#include "stringUtility.h"
#include "Actor.h"
#include "D3D11Mesh.h"
#include "SystemClass.h"
#include "GraphicsClass.h"
#include "D3DClass.h"

const std::string MeshRenderComponent::g_Name = "MeshRenderComponent";

MeshRenderComponent::MeshRenderComponent() {}

const std::string& MeshRenderComponent::VGetName() const {
	return g_Name;
}

const char* MeshRenderComponent::GetPixelShaderResource() {
	return m_pixelShaderResource.c_str();
}

const char* MeshRenderComponent::GetVertexShaderResource() {
	return m_vertexShaderResource.c_str();
}

bool MeshRenderComponent::VDelegateInit(TiXmlElement* pData) {

	TiXmlElement* pPixelShader = pData->FirstChildElement("PixelShader");
	if (pPixelShader) {
		m_pixelShaderResource = pPixelShader->FirstChild()->Value();
	}
	if(m_pixelShaderResource != "NoPShader") {
		ShaderHolder::AddShader(SystemClass::GetGraphics().GetD3D()->GetDevice(), SystemClass::GetGraphics().GetD3D()->GetDeviceContext(), { s2ws(m_pixelShaderResource), ShaderClass::ShaderType::PixelShader });
	}

	TiXmlElement* pVertexShader = pData->FirstChildElement("VertexShader");
	if (pVertexShader) {
		m_vertexShaderResource = pVertexShader->FirstChild()->Value();
	}
	if (m_vertexShaderResource != "NoVShader") {
		ShaderHolder::AddShader(SystemClass::GetGraphics().GetD3D()->GetDevice(), SystemClass::GetGraphics().GetD3D()->GetDeviceContext(), { s2ws(m_vertexShaderResource), ShaderClass::ShaderType::VertexShader });
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
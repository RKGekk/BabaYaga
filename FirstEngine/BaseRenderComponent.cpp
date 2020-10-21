#include "BaseRenderComponent.h"
#include "Actor.h"
#include "IEventManager.h"
#include "EvtData_New_Render_Component.h"
#include "EvtData_Modified_Render_Component.h"
#include "stringUtility.h"

bool BaseRenderComponent::VInit(TiXmlElement* pData) {
    // color
    TiXmlElement* pColorNode = pData->FirstChildElement("Color");
    if (pColorNode) {
        m_color = LoadColor(pColorNode);
    }

    return VDelegateInit(pData);
}

void BaseRenderComponent::VPostInit() {
    std::shared_ptr<SceneNode> pSceneNode(VGetSceneNode());
    std::shared_ptr<EvtData_New_Render_Component> pEvent(new EvtData_New_Render_Component(m_pOwner->GetId(), pSceneNode));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void BaseRenderComponent::VOnChanged() {
    std::shared_ptr<EvtData_Modified_Render_Component> pEvent(new EvtData_Modified_Render_Component(m_pOwner->GetId()));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

std::shared_ptr<SceneNode> BaseRenderComponent::VGetSceneNode() {
    if (!m_pSceneNode) {
        m_pSceneNode = VCreateSceneNode();
    }
    return m_pSceneNode;
}

DirectX::XMFLOAT4 BaseRenderComponent::LoadColor(TiXmlElement* pData) {
    DirectX::XMFLOAT4 color;

    double r = 1.0;
    double g = 1.0;
    double b = 1.0;
    double a = 1.0;

    pData->Attribute("r", &r);
    pData->Attribute("g", &g);
    pData->Attribute("b", &b);
    pData->Attribute("a", &a);

    color.x = (float)r;
    color.y = (float)g;
    color.z = (float)b;
    color.w = (float)a;

    return color;
}

const DirectX::XMFLOAT4 BaseRenderComponent::GetColor() const {
    return m_color;
}

bool BaseRenderComponent::VDelegateInit(TiXmlElement* pData) {
    return true;
}
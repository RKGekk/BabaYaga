#pragma once

#include "LightProperties.h"
#include "BaseRenderComponent.h"

class LightRenderComponent : public BaseRenderComponent {
    LightProperties m_Props;

public:
    static const std::string g_Name;
    virtual const std::string& VGetName() const;

    LightRenderComponent();

    LightProperties GetLight() const;

    void SetAmbient4f(const DirectX::XMFLOAT4& color);
    void SetDiffuse4f(const DirectX::XMFLOAT4& color);
    void SetSpecular4f(const DirectX::XMFLOAT4& color);

protected:
    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual std::shared_ptr<SceneNode> VCreateSceneNode() override;  // factory method to create the appropriate scene node
};
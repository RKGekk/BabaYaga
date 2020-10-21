#pragma once

#include <memory>
#include <DirectXMath.h>

#include "RenderComponentInterface.h"

class BaseRenderComponent : public RenderComponentInterface {
protected:
    DirectX::XMFLOAT4           m_color;
    std::shared_ptr<SceneNode>  m_pSceneNode;

public:
    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VOnChanged() override;
    const DirectX::XMFLOAT4 GetColor() const;

protected:
    // loads the SceneNode specific data (represented in the <SceneNode> tag)
    virtual bool VDelegateInit(TiXmlElement* pData);
    virtual std::shared_ptr<SceneNode> VCreateSceneNode() = 0;  // factory method to create the appropriate scene node
    DirectX::XMFLOAT4 LoadColor(TiXmlElement* pData);

private:
    virtual std::shared_ptr<SceneNode> VGetSceneNode() override;
};
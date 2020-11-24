#pragma once

#include <string>
#include <memory>

#include "BaseRenderComponent.h"

class MeshRenderComponent : public BaseRenderComponent {
    std::string m_pixelShaderResource;
    std::string m_vertexShaderResource;

public:
    static const std::string g_Name;
    virtual const std::string& VGetName() const;

    MeshRenderComponent();
    const char* GetPixelShaderResource();
    const char* GetVertexShaderResource();

protected:
    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual std::shared_ptr<SceneNode> VCreateSceneNode() override;  // factory method to create the appropriate scene node
};
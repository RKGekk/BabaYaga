#pragma once

#include <string>
#include <memory>

#include "BaseRenderComponent.h"

class MeshRenderComponent : public BaseRenderComponent {
    std::string m_textureResource;
    std::string m_objResource;
    std::string m_pixelShaderResource;
    std::string m_vertexShaderResource;

public:
    static const char* g_Name;
    virtual const char* VGetName() const;

    MeshRenderComponent();
    const char* GetTextureResource();
    const char* GetObjResource();
    const char* GetPixelShaderResource();
    const char* GetVertexShaderResource();

protected:
    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual std::shared_ptr<SceneNode> VCreateSceneNode() override;  // factory method to create the appropriate scene node
};
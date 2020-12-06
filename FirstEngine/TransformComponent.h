#pragma once

#include "ActorComponent.h"
#include <DirectXMath.h>

// This component implementation is a very simple representation of the physical aspect of an actor.  It just defines 
// the transform and doesn't register with the physics system at all.
class TransformComponent : public ActorComponent {

private:
    DirectX::XMFLOAT4X4 m_transform;
    DirectX::XMFLOAT4 m_direction;

public:
    static const std::string g_Name;

    TransformComponent();

    virtual bool VInit(TiXmlElement* pData) override;
    virtual const std::string& VGetName() const override;

    // transform functions
    DirectX::XMFLOAT4X4 GetTransform4x4f() const;
    DirectX::XMMATRIX GetTransform() const;

    void SetTransform(const DirectX::XMFLOAT4X4& newTransform);
    void SetTransform(const DirectX::FXMMATRIX& newTransform);

    DirectX::XMFLOAT3 GetPosition3f() const;
    DirectX::XMVECTOR GetPosition() const;

    void SetPosition3f(const DirectX::XMFLOAT3& pos);
    void SetPosition4x4f(const DirectX::XMFLOAT4X4& pos);
    void SetPosition(const DirectX::FXMVECTOR& pos);

    DirectX::XMFLOAT3 GetLookAt3f() const;
    DirectX::XMVECTOR GetLookAt() const;

    DirectX::XMFLOAT3 GetYawPitchRoll3f() const;
    DirectX::XMVECTOR GetYawPitchRoll() const;

    DirectX::XMFLOAT3 GetDirection3f() const;
    DirectX::XMFLOAT4 GetDirection4f() const;
    DirectX::XMVECTOR GetDirection() const;
};

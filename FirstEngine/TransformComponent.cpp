#include "TransformComponent.h"
#include "stringUtility.h"

const char* TransformComponent::g_Name = "TransformComponent";

TransformComponent::TransformComponent(void) {
    DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());
}

const char* TransformComponent::VGetName() const {
    return g_Name;
}

DirectX::XMFLOAT4X4 TransformComponent::GetTransform4x4f() const {
    return m_transform;
}

DirectX::XMMATRIX TransformComponent::GetTransform() const {
    return DirectX::XMLoadFloat4x4(&m_transform);
}

void TransformComponent::SetTransform(const DirectX::XMFLOAT4X4& newTransform) {
    m_transform = newTransform;
}

void TransformComponent::SetTransform(const DirectX::FXMMATRIX& newTransform) {
    DirectX::XMStoreFloat4x4(&m_transform, newTransform);
}

DirectX::XMFLOAT3 TransformComponent::GetPosition3f() const {
    return DirectX::XMFLOAT3(m_transform.m[3][0], m_transform.m[3][1], m_transform.m[3][2]);
}

DirectX::XMVECTOR TransformComponent::GetPosition() const {
    return DirectX::XMVectorSet(m_transform.m[3][0], m_transform.m[3][1], m_transform.m[3][2], 1.0f);
}

void TransformComponent::SetPosition3f(const DirectX::XMFLOAT3& pos) {
    m_transform.m[3][0] = pos.x;
    m_transform.m[3][1] = pos.y;
    m_transform.m[3][2] = pos.z;
    m_transform.m[3][3] = 1.0f;
}

void TransformComponent::SetPosition4x4f(const DirectX::XMFLOAT4X4& pos) {
    m_transform.m[3][0] = pos.m[3][0];
    m_transform.m[3][1] = pos.m[3][1];
    m_transform.m[3][2] = pos.m[3][2];
    m_transform.m[3][3] = 1.0f;
}

void TransformComponent::SetPosition(const DirectX::FXMVECTOR& pos) {
    DirectX::XMFLOAT3 temp;
    DirectX::XMStoreFloat3(&temp, pos);
    m_transform.m[3][0] = temp.x;
    m_transform.m[3][1] = temp.y;
    m_transform.m[3][2] = temp.z;
    m_transform.m[3][3] = 1.0f;
}

DirectX::XMFLOAT3 TransformComponent::GetLookAt3f() const {
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, GetLookAt());

    return result;
}

DirectX::XMVECTOR TransformComponent::GetLookAt() const {
    DirectX::XMFLOAT4X4 justRot4x4 = m_transform;
    justRot4x4.m[3][0] = 0.0f;
    justRot4x4.m[3][1] = 0.0f;
    justRot4x4.m[3][2] = 0.0f;
    justRot4x4.m[3][3] = 1.0f;
    DirectX::XMMATRIX justRot = DirectX::XMLoadFloat4x4(&justRot4x4);

    DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR out = DirectX::XMVector4Transform(forward, justRot);

    return out;
}

DirectX::XMFLOAT3 TransformComponent::GetYawPitchRoll3f() const {

    float pitch = DirectX::XMScalarASin(-m_transform._32);
    float roll = DirectX::XMVectorGetX(
        DirectX::XMVectorATan2(
            DirectX::XMVectorReplicate(m_transform._12),
            DirectX::XMVectorReplicate(m_transform._22)
        )
    );
    float yaw = DirectX::XMVectorGetX(
        DirectX::XMVectorATan2(
            DirectX::XMVectorReplicate(m_transform._31),
            DirectX::XMVectorReplicate(m_transform._33)
        )
    );
    return DirectX::XMFLOAT3(yaw, pitch, roll);
}

DirectX::XMVECTOR TransformComponent::GetYawPitchRoll() const {

    DirectX::XMFLOAT3 yawPitchRoll = GetYawPitchRoll3f();
    return DirectX::XMLoadFloat3(&yawPitchRoll);
}

bool TransformComponent::VInit(TiXmlElement* pData) {

    // The TransformComponents can come in partial definitions,
    // such as from the editor, its better to grab the current values rather than clear them out.

    DirectX::XMFLOAT3 position;
    TiXmlElement* pPositionElement = pData->FirstChildElement("Position");
    if (pPositionElement) {
        double x = 0;
        double y = 0;
        double z = 0;
        pPositionElement->Attribute("x", &x);
        pPositionElement->Attribute("y", &y);
        pPositionElement->Attribute("z", &z);
        position = DirectX::XMFLOAT3(x, y, z);
    }
    else {
        position = GetPosition3f();
    }

    DirectX::XMFLOAT3 yawPitchRoll;
    TiXmlElement* pOrientationElement = pData->FirstChildElement("YawPitchRoll");
    if (pOrientationElement) {
        double yaw = 0;
        double pitch = 0;
        double roll = 0;
        pOrientationElement->Attribute("x", &yaw);
        pOrientationElement->Attribute("y", &pitch);
        pOrientationElement->Attribute("z", &roll);
        yawPitchRoll = DirectX::XMFLOAT3(DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(roll));
    }
    else {
        yawPitchRoll = GetYawPitchRoll3f();
    }

    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(yawPitchRoll.y, yawPitchRoll.x, yawPitchRoll.z);

    DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixMultiply(rotation, translation));

    return true;
}
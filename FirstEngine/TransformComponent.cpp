#include "TransformComponent.h"
#include "stringUtility.h"

const std::string TransformComponent::g_Name = "TransformComponent";

TransformComponent::TransformComponent() {

    m_direction = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());
}

const std::string& TransformComponent::VGetName() const {
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

DirectX::XMFLOAT3 TransformComponent::GetScale3f() const {
    return DirectX::XMFLOAT3(m_scale.x, m_scale.y, m_scale.z);
}

DirectX::XMVECTOR TransformComponent::GetScale() const {
    return DirectX::XMVectorSet(m_scale.x, m_scale.y, m_scale.z, 1.0f);
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

void TransformComponent::SetScale3f(const DirectX::XMFLOAT3& sclae) {
    m_scale.x = sclae.x;
    m_scale.y = sclae.y;
    m_scale.z = sclae.z;
    m_scale.w = 1.0f;
}

void TransformComponent::SetScale(const DirectX::FXMVECTOR& scale) {
    DirectX::XMFLOAT3 temp;
    DirectX::XMStoreFloat3(&temp, scale);
    m_scale.x = temp.x;
    m_scale.y = temp.y;
    m_scale.z = temp.z;
    m_scale.w = 1.0f;
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

DirectX::XMFLOAT3 TransformComponent::GetDirection3f() const {
    return DirectX::XMFLOAT3(m_direction.x, m_direction.y, m_direction.z);
}

DirectX::XMFLOAT4 TransformComponent::GetDirection4f() const {
    return m_direction;
}

DirectX::XMVECTOR TransformComponent::GetDirection() const {
    DirectX::XMVECTOR directionXM = DirectX::XMLoadFloat4(&m_direction);
    return directionXM;
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
        const char* at = pOrientationElement->Attribute("angleType");
        std::string angleType(at == nullptr? "" : at);
        if(angleType == "rad") {
            yawPitchRoll = DirectX::XMFLOAT3(yaw, pitch, roll);
        }
        else {
            yawPitchRoll = DirectX::XMFLOAT3(DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(roll));
        }
    }
    else {
        yawPitchRoll = GetYawPitchRoll3f();
    }

    DirectX::XMFLOAT4 direction;
    TiXmlElement* pDirectionElement = pData->FirstChildElement("Direction");
    if (pDirectionElement) {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        pDirectionElement->Attribute("x", &x);
        pDirectionElement->Attribute("y", &y);
        pDirectionElement->Attribute("z", &z);
        direction = DirectX::XMFLOAT4(x, y, z, 0.0f);
    }
    else {
        direction = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    DirectX::XMFLOAT4 scale;
    TiXmlElement* pScaleElement = pData->FirstChildElement("Scale");
    if (pScaleElement) {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        pScaleElement->Attribute("x", &x);
        pScaleElement->Attribute("y", &y);
        pScaleElement->Attribute("z", &z);
        scale = DirectX::XMFLOAT4(x, y, z, 1.0f);
    }
    else {
        scale = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    DirectX::XMMATRIX translationXM = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX rotationXM = DirectX::XMMatrixRotationRollPitchYaw(yawPitchRoll.y, yawPitchRoll.x, yawPitchRoll.z);
    //DirectX::XMVECTOR directionXM = DirectX::XMLoadFloat4(&direction);

    //DirectX::XMStoreFloat4(&m_direction, DirectX::XMVector4Transform(DirectX::XMVector4Normalize(directionXM), rotationXM));
    m_direction = direction;
    m_scale = scale;
    DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixMultiply(rotationXM, translationXM));

    return true;
}
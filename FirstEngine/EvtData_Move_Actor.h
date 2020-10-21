#pragma once

#include <DirectXMath.h>
#include <memory>

#include "BaseEventData.h"

// Sent when actors are moved
class EvtData_Move_Actor : public BaseEventData {
    unsigned int m_id;
    DirectX::XMFLOAT4X4 m_matrix;
    const std::string m_eventName;

public:
    static const EventTypeId sk_EventType = 0xeeaa0a40;

    virtual const EventTypeId& VGetEventType() const {
        return sk_EventType;
    }

    EvtData_Move_Actor() {
        m_id = 0;
    }

    EvtData_Move_Actor(unsigned int id, const DirectX::XMFLOAT4X4& matrix) : m_eventName("EvtData_Move_Actor") {
        m_id = id;
        m_matrix = matrix;
    }

    virtual void VSerialize(std::ostream& out) const {
        out << m_id << " ";
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out << m_matrix.m[i][j] << " ";
            }
        }
    }

    virtual void VDeserialize(std::istream& in) {
        in >> m_id;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                in >> m_matrix.m[i][j];
            }
        }
    }

    virtual IEventDataPtr VCopy() const {
        return IEventDataPtr(new EvtData_Move_Actor(m_id, m_matrix));
    }

    virtual const std::string& GetName(void) const {
        return m_eventName;
    }

    unsigned int GetId() const {
        return m_id;
    }

    const DirectX::XMFLOAT4X4& GetMatrix() const {
        return m_matrix;
    }
};
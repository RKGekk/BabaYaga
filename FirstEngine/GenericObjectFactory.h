#pragma once

#include <map>

template <class BaseClass, class IdType>
class GenericObjectFactory {

    std::map<IdType, BaseClass* (*)()> m_creationFunctions;

public:
    template <class SubClass>
    bool Register(IdType id) {
        auto findIt = m_creationFunctions.find(id);
        if (findIt == m_creationFunctions.end()) {
            m_creationFunctions[id] = []()->BaseClass* { return new SubClass; };
            return true;
        }

        return false;
    }

    BaseClass* Create(IdType id) {
        auto findIt = m_creationFunctions.find(id);
        if (findIt != m_creationFunctions.end()) {
            auto pFunc = findIt->second;
            return pFunc();
        }

        return NULL;
    }
};
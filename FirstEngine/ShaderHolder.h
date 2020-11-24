#pragma once

#include <map>
#include <memory>
#include <string>

#include "ShaderClass.h"

class ShaderHolder {
private:
    std::map<std::wstring, ShaderClass> m_Shaders;
    static ShaderHolder* m_s_Instance;

    static bool LoadShader(ID3D11Device*, ID3D11DeviceContext*, const std::wstring&, ShaderClass::ShaderType st);

public:

    struct ShaderProperty {
        std::wstring            fileName;
        ShaderClass::ShaderType shaderType;
    };

    ShaderHolder();
    ShaderHolder(const ShaderHolder&) = delete;
    ShaderHolder& operator=(const ShaderHolder&) = delete;

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const std::vector<ShaderProperty>&);

    static bool AddShader(ID3D11Device*, ID3D11DeviceContext*, const ShaderProperty&);
    static ShaderClass& GetShader(std::wstring const& filename);
};
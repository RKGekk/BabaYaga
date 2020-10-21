#pragma once

#include <map>
#include <memory>
#include <string>

#include "TextureClass.h"

class TextureHolder {
private:
    std::map<std::string, TextureClass> m_Textures;
    static TextureHolder*               m_s_Instance;

    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, const std::string&);

public:
    TextureHolder();
    TextureHolder(const TextureHolder&) = delete;
    TextureHolder& operator=(const TextureHolder&) = delete;

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const std::vector<std::string>&);
    void AddBlankSoftTexture(unsigned int width, unsigned int height, const std::string& name, ColorIntegers color);

    static TextureClass& GetTexture(std::string const& filename);
};

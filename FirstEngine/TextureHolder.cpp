#include "TextureHolder.h"

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder() {
	m_s_Instance = this;
}

bool TextureHolder::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<std::string>& textureFilenames) {
	bool result;

	for (const std::string& textureFilename : textureFilenames) {
		result = AddTexture(device, deviceContext, textureFilename);
		if (!result) {
			return false;
		}
	}

	return true;
}

bool TextureHolder::AddTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& textureFilename) {
	if (m_s_Instance->m_Textures.count(textureFilename)) {
		return true;
	}
	bool result = LoadTexture(device, deviceContext, textureFilename);
	if (!result) {
		return false;
	}

	result = m_s_Instance->m_Textures[textureFilename].LoadToGPU(device, deviceContext, D3D11_USAGE_DEFAULT, 0);
	if (!result) {
		return false;
	}
	return true;
}

void TextureHolder::AddBlankSoftTexture(unsigned int width, unsigned int height, const std::string& name, ColorIntegers color) {

	// Create the texture object.
	auto& m = m_s_Instance->m_Textures;
	auto& texture = m[name];

	// Initialize the texture object.
	texture.InitializeBlank(width, height, color);
}

bool TextureHolder::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filename) {
	bool result;

	// Create the texture object.
	auto& m = m_s_Instance->m_Textures;
	auto& texture = m[filename];

	// Initialize the texture object.
	result = texture.Initialize(filename);
	if (!result) {
		return false;
	}

	return true;
}

TextureClass& TextureHolder::GetTexture(std::string const& filename) {
	auto& m = m_s_Instance->m_Textures;
	auto keyValuePair = m.find(filename);
	if (keyValuePair != m.end()) {
		return keyValuePair->second;
	}
	else {
		throw;
	}
}
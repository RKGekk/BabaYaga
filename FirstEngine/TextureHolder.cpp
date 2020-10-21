#include "TextureHolder.h"

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder() {
	m_s_Instance = this;
}

bool TextureHolder::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<std::string>& textureFilenames) {
	bool result;

	for (const std::string& textureFilename : textureFilenames) {
		result = LoadTexture(device, deviceContext, textureFilename);
		if (!result) {
			return false;
		}

		// Upload picture data to GPU memory
		result = m_s_Instance->m_Textures[textureFilename].LoadToGPU(device, deviceContext, D3D11_USAGE_DEFAULT, 0);
		if (!result) {
			return false;
		}
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
	// auto is equivalent of map<string, Texture>::iterator
	// Did we find a match?
	if (keyValuePair != m.end()) {
		return keyValuePair->second;
	}
	else {
		throw;
	}
}
#include "ShaderHolder.h"

ShaderHolder* ShaderHolder::m_s_Instance = nullptr;

ShaderHolder::ShaderHolder() {
	m_s_Instance = this;
}

bool ShaderHolder::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<ShaderProperty>& shaderFilenames) {
	bool result;

	for (const auto& shaderFilename : shaderFilenames) {
		result = AddShader(device, deviceContext, shaderFilename);
		if (!result) {
			return false;
		}
	}

	return true;
}

bool ShaderHolder::AddShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ShaderProperty& shaderFilename) {
	if(m_s_Instance->m_Shaders.count(shaderFilename.fileName)) {
		return true;
	}
	bool result = LoadShader(device, deviceContext, shaderFilename.fileName, shaderFilename.shaderType);
	if (!result) {
		return false;
	}

	// Upload shader data to GPU memory
	result = m_s_Instance->m_Shaders[shaderFilename.fileName].LoadToGPU(device, deviceContext);
	if (!result) {
		return false;
	}
	return true;
}

bool ShaderHolder::LoadShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filename, ShaderClass::ShaderType st) {
	bool result;

	// Create the shader object.
	auto& m = m_s_Instance->m_Shaders;
	auto& shader = m[filename];

	// Initialize the shader object.
	result = shader.Initialize(filename, st);
	if (!result) {
		return false;
	}

	return true;
}

ShaderClass& ShaderHolder::GetShader(std::wstring const& filename) {
	auto& m = m_s_Instance->m_Shaders;
	auto keyValuePair = m.find(filename);
	if (keyValuePair != m.end()) {
		return keyValuePair->second;
	}
	else {
		throw;
	}
}
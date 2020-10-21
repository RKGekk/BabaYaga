#include "ShaderClass.h"

ShaderClass::ShaderClass() {
	m_ShaderType = ShaderType::PixelShader;
	m_EntryPoint = "PixelShaderFX";
	m_CompilerTarget = "ps_5_0";
}

ShaderClass::ShaderClass(ShaderType st) {
	SetShaderType(st);
}

ShaderClass::ShaderClass(ShaderType st, std::string EntryPoint) {
	m_ShaderType = st;
	m_EntryPoint = EntryPoint;
}

ShaderClass::~ShaderClass() {}

bool ShaderClass::Initialize(const std::wstring& filename) {
	HRESULT hResult;

	// Initialize the pointers this function will use to null.
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexErrorMessage;

	std::wstring fileExtension = filename.substr(filename.find_last_of(L".") + 1);
	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::toupper);

	bool result = false;
	if (fileExtension == L"FX") {
		hResult = D3DCompileFromFile(
			filename.c_str(),
			NULL,
			NULL,
			m_EntryPoint.c_str(),
			m_CompilerTarget.c_str(),
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			pBytecodeBlob.GetAddressOf(),
			vertexErrorMessage.GetAddressOf()
		);
		if (FAILED(hResult)) {
			// If the shader failed to compile it should have writen something to the error message.
			if (vertexErrorMessage) {
				OutputShaderErrorMessage(vertexErrorMessage.Get());
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else {
				ThrowIfFailed(hResult);
			}
			result = false;
		}
		else {
			result = true;
		}
	}
	else if (fileExtension == L"CSO") {
		hResult = D3DReadFileToBlob(filename.c_str(), &pBytecodeBlob);
		if (FAILED(hResult)) {
			ThrowIfFailed(hResult);
			result = false;
		}
		else {
			result = true;
		}
	}

	return result;
}

bool ShaderClass::Initialize(const std::wstring& filename, ShaderType st) {
	SetShaderType(st);
	return Initialize(filename);
}

bool ShaderClass::Initialize(const std::wstring& filename, ShaderType st, std::string EntryPoint) {
	m_ShaderType = st;
	m_EntryPoint = EntryPoint;
	return Initialize(filename);
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage) {
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	//MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

void ShaderClass::SetShaderType(ShaderType st) {
	m_ShaderType = st;
	switch (st) {
		case ShaderClass::ShaderType::ComputeShader: {
			m_EntryPoint = "ComputeShaderFX";
			m_CompilerTarget = "cs_5_0";
		}
		break;
		case ShaderClass::ShaderType::DomainShader: {
			m_EntryPoint = "DomainShaderFX";
			m_CompilerTarget = "ds_5_0";
		}
		break;
		case ShaderClass::ShaderType::GeometryShader: {
			m_EntryPoint = "GeometryShaderFX";
			m_CompilerTarget = "gs_5_0";
		}
		break;
		case ShaderClass::ShaderType::HullShader: {
			m_EntryPoint = "HullShaderFX";
			m_CompilerTarget = "hs_5_0";
		}
		break;
		case ShaderClass::ShaderType::PixelShader: {
			m_EntryPoint = "PixelShaderFX";
			m_CompilerTarget = "ps_5_0";
		}
		break;
		case ShaderClass::ShaderType::VertexShader: {
			m_EntryPoint = "VertexShaderFX";
			m_CompilerTarget = "vs_5_0";
		}
		break;
		default: {
			m_EntryPoint = "PixelShaderFX";
			m_CompilerTarget = "ps_5_0";
		}
		break;
	}
}

bool ShaderClass::LoadToGPU(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	HRESULT hResult;

	bool result = false;
	switch (m_ShaderType) {
		case ShaderClass::ShaderType::ComputeShader: {
			hResult = device->CreateComputeShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pComputeShader.GetAddressOf());
		}
		break;
		case ShaderClass::ShaderType::DomainShader: {
			hResult = device->CreateDomainShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pDomainShader.GetAddressOf());
		}
		break;
		case ShaderClass::ShaderType::GeometryShader: {
			hResult = device->CreateGeometryShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pGeometryShader.GetAddressOf());
		}
		break;
		case ShaderClass::ShaderType::HullShader: {
			hResult = device->CreateHullShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pHullShader.GetAddressOf());
		}
		break;
		case ShaderClass::ShaderType::PixelShader: {
			hResult = device->CreatePixelShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pPixelShader.GetAddressOf());
		}
		break;
		case ShaderClass::ShaderType::VertexShader: {
			hResult = device->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pVertexShader.GetAddressOf());
		}
		break;
		default: {
			hResult = device->CreatePixelShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, pPixelShader.GetAddressOf());
		}
		break;
	}

	if (FAILED(hResult)) {
		ThrowIfFailed(hResult);
		result = false;
	}
	else {
		result = true;
	}

	return result;
}

ID3DBlob* ShaderClass::GetBytecode() const {
	return pBytecodeBlob.Get();
}

ID3D11ComputeShader* ShaderClass::GetComputeShader() const {
	return pComputeShader.Get();
}

ID3D11DomainShader* ShaderClass::GetDomainShader() const {
	return pDomainShader.Get();
}

ID3D11GeometryShader* ShaderClass::GetGeometryShader() const {
	return pGeometryShader.Get();
}

ID3D11HullShader* ShaderClass::GetHullShader() const {
	return pHullShader.Get();
}

ID3D11PixelShader* ShaderClass::GetPixelShader() const {
	return pPixelShader.Get();
}

ID3D11VertexShader* ShaderClass::GetVertexShader() const {
	return pVertexShader.Get();
}

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include <fstream>

#include "memoryUtility.h"

class ShaderClass {
public:

	enum class ShaderType {
		ComputeShader,
		DomainShader,
		GeometryShader,
		HullShader,
		PixelShader,
		VertexShader
	};

	ShaderClass();
	ShaderClass(ShaderType st);
	ShaderClass(ShaderType st, std::string EntryPoint);
	ShaderClass(const ShaderClass&) = delete;
	ShaderClass& operator=(const ShaderClass&) = delete;
	~ShaderClass();

	bool Initialize(const std::wstring&);
	bool Initialize(const std::wstring&, ShaderType st);
	bool Initialize(const std::wstring&, ShaderType st, std::string EntryPoint);
	bool LoadToGPU(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	ID3DBlob* GetBytecode() const;

	ID3D11ComputeShader*	GetComputeShader()	const;
	ID3D11DomainShader*		GetDomainShader()	const;
	ID3D11GeometryShader*	GetGeometryShader()	const;
	ID3D11HullShader*		GetHullShader()		const;
	ID3D11PixelShader*		GetPixelShader()	const;
	ID3D11VertexShader*		GetVertexShader()	const;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob>				pBytecodeBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>		pComputeShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		pDomainShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	pGeometryShader;
	Microsoft::WRL::ComPtr<ID3D11HullShader>		pHullShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		pVertexShader;

	ShaderType	m_ShaderType;
	std::string	m_EntryPoint;
	std::string	m_CompilerTarget;

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage);
	void SetShaderType(ShaderType st);
};
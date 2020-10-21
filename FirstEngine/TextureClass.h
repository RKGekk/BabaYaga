#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <wrl.h>
#include <gdiplus.h>

#include "stringUtility.h"
#include "memoryUtility.h"
#include "ColorIntegers.h"
#include "DDSTextureLoader.h"

class TextureClass {
private:
	struct TargaHeader {
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&) = delete;
	TextureClass& operator=(const TextureClass&) = delete;
	~TextureClass();

	bool Initialize(const std::string&);
	bool InitializeBlank(unsigned int width, unsigned int height, ColorIntegers color);

	bool LoadToGPU(ID3D11Device* device, ID3D11DeviceContext* deviceContext, D3D11_USAGE usage, UINT cpuAccess);
	void Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void Clear(ColorIntegers color);
	bool CopyNoLoad(const TextureClass& src);

	void PutPixel(unsigned int x, unsigned int y, ColorIntegers c);
	void PutPixelAlphaBlend(unsigned int x, unsigned int y, ColorIntegers c);
	ColorIntegers GetPixel(unsigned int x, unsigned int y) const;
	void DrawLine(float x1, float y1, float x2, float y2, ColorIntegers c);

	ID3D11ShaderResourceView* GetTexture();

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

private:
	bool LoadTga(const std::string&);
	bool LoadBmp(const std::string&);
	bool LoadDds(const std::string&);

	std::vector<unsigned char>							m_Data;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_textureView;

	unsigned int m_width;
	unsigned int m_height;
};
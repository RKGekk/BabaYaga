#include "textureclass.h"

TextureClass::TextureClass() {

}

TextureClass::~TextureClass() {}

bool TextureClass::Initialize(const std::string& filename) {
	HRESULT hResult;

	// Load the targa image data into memory.
	int height;
	int width;

	std::string fileExtension = filename.substr(filename.find_last_of(".") + 1);
	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::toupper);

	bool result;
	if (fileExtension == "TGA") {
		result = LoadTga(filename);
	}
	else if (fileExtension == "BMP") {
		result = LoadBmp(filename);
	}
	else {
		return false;
	}
	if (!result) {
		return false;
	}

	return true;
}

bool TextureClass::InitializeBlank(unsigned int width, unsigned int height, ColorIntegers color) {
	HRESULT hResult;

	m_width = width;
	m_height = height;
	m_Data = std::vector<unsigned char>(width * height * 4);
	Clear(color);

	return true;
}

bool TextureClass::LoadToGPU(ID3D11Device* device, ID3D11DeviceContext* deviceContext, D3D11_USAGE usage, UINT cpuAccess) {

	HRESULT hResult;

	// Setup the description of the texture.
	D3D11_TEXTURE2D_DESC textureDesc;
	if (usage == D3D11_USAGE_DYNAMIC) {
		textureDesc.Width = m_height;
		textureDesc.Height = m_width;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = usage;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = cpuAccess;
		textureDesc.MiscFlags = 0;
	}
	else {
		textureDesc.Height = m_height;
		textureDesc.Width = m_width;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = usage;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = cpuAccess; //D3D11_CPU_ACCESS_FLAG
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	// Create the empty texture.
	ThrowIfFailed(
		device->CreateTexture2D(&textureDesc, NULL, m_texture.GetAddressOf())
	);
	

	// Set the row pitch of the targa image data.
	unsigned int rowPitch = (m_width * 4) * sizeof(unsigned char);

	// Copy the image data into the texture.
	deviceContext->UpdateSubresource(m_texture.Get(), 0, NULL, m_Data.data(), rowPitch, 0);

	// Setup the shader resource view description.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_textureView.GetAddressOf());
	if (FAILED(hResult)) {
		return false;
	}

	if (textureDesc.MiscFlags != 0) {
		// Generate mipmaps for this texture.
		deviceContext->GenerateMips(m_textureView.Get());
	}

	return true;
}

void TextureClass::Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	D3D11_MAPPED_SUBRESOURCE mappedSysBufferTexture;

	// lock and map the adapter memory for copying over the sysbuffer
	ThrowIfFailed(
		deviceContext->Map(m_texture.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSysBufferTexture)
	);

	// perform the copy line-by-line
	unsigned char* v = reinterpret_cast<unsigned char*>(mappedSysBufferTexture.pData);
	std::copy(m_Data.begin(), m_Data.end(), v);

	// release the adapter memory
	deviceContext->Unmap(m_texture.Get(), 0u);
}

void TextureClass::Clear(ColorIntegers color) {

	unsigned int* v = reinterpret_cast<unsigned int*>(m_Data.data());
	std::fill(v, v + (m_Data.size() / 4), color.dword);

	/*
	for (UINT i = 0; i < m_Data.size() / 4; ++i) {
		v[i] = color.dword;
	}*/
}

bool TextureClass::CopyNoLoad(const TextureClass& src) {

	size_t sz = src.m_Data.size();
	m_Data.clear();
	m_Data.reserve(sz);
	std::memcpy(m_Data.data(), src.m_Data.data(), sz);

	return true;
}

void TextureClass::PutPixel(unsigned int x, unsigned int y, ColorIntegers c) {

	unsigned int* v = reinterpret_cast<unsigned int*>(m_Data.data());
	v[y * m_width + x] = c.dword;
}

void TextureClass::PutPixelAlphaBlend(unsigned int x, unsigned int y, ColorIntegers c) {

	// load source pixel
	const ColorIntegers d = GetPixel(x, y);

	// blend channels
	const unsigned char rsltRed = (c.GetR() * c.GetA() + d.GetR() * (255u - c.GetA())) / 256u;
	const unsigned char rsltGreen = (c.GetG() * c.GetA() + d.GetG() * (255u - c.GetA())) / 256u;
	const unsigned char rsltBlue = (c.GetB() * c.GetA() + d.GetB() * (255u - c.GetA())) / 256u;

	// pack channels back into pixel and fire pixel onto surface
	PutPixel(x, y, {rsltRed, rsltGreen, rsltBlue});
}

ColorIntegers TextureClass::GetPixel(unsigned int x, unsigned int y) const {
	unsigned int* v = const_cast<unsigned int*>(reinterpret_cast<const unsigned int*>(m_Data.data()));
	return v[y * m_width + x];
}

void TextureClass::DrawLine(float x1, float y1, float x2, float y2, ColorIntegers c) {

	const float dx = x2 - x1;
	const float dy = y2 - y1;

	if (dy == 0.0f && dx == 0.0f) {
		PutPixel(int(x1), int(y1), c);
	}
	else if (abs(dy) > abs(dx)) {
		if (dy < 0.0f) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		const float m = dx / dy;
		float y = y1;
		int lastIntY;
		for (float x = x1; y < y2; y += 1.0f, x += m) {
			lastIntY = int(y);
			PutPixel(int(x), lastIntY, c);
		}
		if (int(y2) > lastIntY) {
			PutPixel(int(x2), int(y2), c);
		}
	}
	else {
		if (dx < 0.0f) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		const float m = dy / dx;
		float x = x1;
		int lastIntX;
		for (float y = y1; x < x2; x += 1.0f, y += m) {
			lastIntX = int(x);
			PutPixel(lastIntX, int(y), c);
		}
		if (int(x2) > lastIntX) {
			PutPixel(int(x2), int(y2), c);
		}
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture() {
	return m_textureView.Get();
}

unsigned int TextureClass::GetWidth() const {
	return m_width;
}

unsigned int TextureClass::GetHeight() const {
	return m_height;
}

bool TextureClass::LoadTga(const std::string& filename) {

	unsigned int count;
	TargaHeader targaFileHeader;

	// Open the targa file for reading in binary.
	FILE* filePtr;
	int error = fopen_s(&filePtr, filename.c_str(), "rb");
	if (error != 0) {
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Get the important information from the header.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32) {
		return false;
	}

	// Calculate the size of the 32 bit image data.
	int imageSize = m_width * m_height * 4;

	// Allocate memory for the targa image data.
	std::vector<unsigned char> targaImage(imageSize);

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage.data(), 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	// Allocate memory for the targa destination data.
	m_Data = std::vector<unsigned char>(imageSize);

	// Initialize the index into the targa destination data array.
	int index = 0;

	// Initialize the index into the targa image data.
	int k = (m_width * m_height * 4) - (m_width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (int j = 0; j < m_height; j++) {
		for (int i = 0; i < m_width; i++) {
			m_Data[index + 0] = targaImage[k + 2];  // Red.
			m_Data[index + 1] = targaImage[k + 1];  // Green.
			m_Data[index + 2] = targaImage[k + 0];  // Blue
			m_Data[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	return true;
}

bool TextureClass::LoadBmp(const std::string& filename) {
	
	unsigned int pitch = 0;
	
	Gdiplus::Bitmap bitmap(s2ws(filename).c_str());
	Gdiplus::Status st = bitmap.GetLastStatus();
	if (st != Gdiplus::Status::Ok) {
		return false;
	}

	m_width = bitmap.GetWidth();
	m_height = bitmap.GetHeight();

	// Calculate the size of the 32 bit image data.
	int imageSize = m_width * m_height * 4;

	// Initialize the index into the targa destination data array.
	int index = 0;

	m_Data = std::vector<unsigned char>(imageSize);

	for (unsigned int y = 0; y < m_height; y++) {
		for (unsigned int x = 0; x < m_width; x++) {
			Gdiplus::Color c;
			bitmap.GetPixel(x, y, &c);
			m_Data[index + 0] = c.GetR();
			m_Data[index + 1] = c.GetG();
			m_Data[index + 2] = c.GetB();
			m_Data[index + 3] = c.GetA();

			index += 4;
		}
	}

	return true;
}

bool TextureClass::LoadDds(const std::string& filename) {

	

	return true;
}

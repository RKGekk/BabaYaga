#pragma once

class ColorIntegers {
public:
	unsigned int dword;

	constexpr ColorIntegers() : dword() {}
	constexpr ColorIntegers(const ColorIntegers& col) : dword(col.dword) {}
	constexpr ColorIntegers(unsigned int dw) : dword(dw) {}
	constexpr ColorIntegers(unsigned char x, unsigned char r, unsigned char g, unsigned char b) : dword((x << 24u) | (r << 16u) | (g << 8u) | b) {}
	constexpr ColorIntegers(unsigned char r, unsigned char g, unsigned char b) : dword((r << 16u) | (g << 8u) | b) {}
	constexpr ColorIntegers(ColorIntegers col, unsigned char x) : ColorIntegers((x << 24u) | col.dword) {}

	ColorIntegers& operator =(ColorIntegers color) {
		dword = color.dword;
		return *this;
	}

	constexpr unsigned char GetX() const {
		return dword >> 24u;
	}

	constexpr unsigned char GetA() const {
		return GetX();
	}

	constexpr unsigned char GetR() const {
		return (dword >> 16u) & 0xFFu;
	}

	constexpr unsigned char GetG() const {
		return (dword >> 8u) & 0xFFu;
	}

	constexpr unsigned char GetB() const {
		return dword & 0xFFu;
	}

	void SetX(unsigned char x) {
		dword = (dword & 0xFFFFFFu) | (x << 24u);
	}

	void SetA(unsigned char a) {
		SetX(a);
	}

	void SetR(unsigned char r) {
		dword = (dword & 0xFF00FFFFu) | (r << 16u);
	}

	void SetG(unsigned char g) {
		dword = (dword & 0xFFFF00FFu) | (g << 8u);
	}

	void SetB(unsigned char b) {
		dword = (dword & 0xFFFFFF00u) | b;
	}
};

namespace IntColors {
	static constexpr ColorIntegers MakeRGB(unsigned char r, unsigned char g, unsigned char b) {
		return (r << 16) | (g << 8) | b;
	}
	static constexpr ColorIntegers White = MakeRGB(255u, 255u, 255u);
	static constexpr ColorIntegers Black = MakeRGB(0u, 0u, 0u);
	static constexpr ColorIntegers Gray = MakeRGB(0x80u, 0x80u, 0x80u);
	static constexpr ColorIntegers LightGray = MakeRGB(0xD3u, 0xD3u, 0xD3u);
	static constexpr ColorIntegers Red = MakeRGB(255u, 0u, 0u);
	static constexpr ColorIntegers Green = MakeRGB(0u, 255u, 0u);
	static constexpr ColorIntegers Blue = MakeRGB(0u, 0u, 255u);
	static constexpr ColorIntegers Yellow = MakeRGB(255u, 255u, 0u);
	static constexpr ColorIntegers Cyan = MakeRGB(0u, 255u, 255u);
	static constexpr ColorIntegers Magenta = MakeRGB(255u, 0u, 255u);
}
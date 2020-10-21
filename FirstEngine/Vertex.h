#pragma once

#include <DirectXMath.h>

class Vertex {
public:
	Vertex() = default;
	Vertex(const DirectX::XMFLOAT4& pos) : pos(pos), n(), t(), color(), tg() {}
	Vertex(const DirectX::XMFLOAT4& pos, const Vertex& src) : pos(pos), n(src.n), t(), color(), tg() {}
	Vertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT3& n) : pos(pos), n(n), t(), color(), tg() {}
	Vertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& t) : pos(pos), n(n), t(t), color(), tg() {}
	Vertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& t, const DirectX::XMFLOAT3& tg) : pos(pos), n(n), t(t), color(), tg(tg) {}

public:
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 n;
	DirectX::XMFLOAT2 t;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 tg;
};
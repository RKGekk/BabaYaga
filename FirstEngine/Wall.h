#pragma once

#include <DirectXMath.h>

#include "TextureClass.h"
#include "Vertex.h"
#include "IndexedTriangleList.h"

struct Wall {
	const TextureClass* pTex;
	IndexedTriangleList model;
	DirectX::XMFLOAT4X4 world;
};
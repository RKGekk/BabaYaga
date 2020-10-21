#pragma once

#include <DirectXMath.h>

#include "Material.h"

struct cbPerObject {

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 worldInvTranspose;
	Material			material;
};
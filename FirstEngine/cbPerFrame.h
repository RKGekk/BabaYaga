#pragma once

#include <DirectXMath.h>

#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

struct cbPerFrame {

	DirectionalLight	dirLight;
	//PointLight		pointLight;
	SpotLight			spotLight;
	DirectX::XMFLOAT3	eyePos;

	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};
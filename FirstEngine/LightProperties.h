#pragma once

#include <DirectXMath.h>

#include "LightType.h"

struct LightProperties {
	LightType m_LightType;
	DirectX::XMFLOAT4 m_Ambient;
	DirectX::XMFLOAT4 m_Diffuse;
	DirectX::XMFLOAT4 m_Specular;
	float m_Attenuation[3];  /* Attenuation coefficients */
	float m_Range;
	float m_Spot;
};
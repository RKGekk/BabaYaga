#pragma once

#include <Windows.h>
#include <DirectXMath.h>

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.
struct SpotLight {
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	DirectX::XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	DirectX::XMFLOAT3 Direction;
	float Spot;

	// Packed into 4D vector: (Att, Pad)
	DirectX::XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};
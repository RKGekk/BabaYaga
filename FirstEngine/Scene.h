#pragma once

#include <string>
#include <d3d11.h>

#include "InputClass.h"
#include "Mouse.h"

class Scene {
public:
	Scene(const std::string& name) : name(name) {}
	virtual ~Scene() = default;

	virtual void Update(ID3D11DeviceContext* deviceContext, InputClass& kbd, Mouse& mouse, float dt, float tt) = 0;
	virtual void Draw(ID3D11DeviceContext* deviceContext) = 0;

	const std::string& GetName() const;

private:
	std::string name;
};
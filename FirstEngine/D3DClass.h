#pragma once

#include <windows.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "EngineOptions.h"

class D3DClass {
public:
	D3DClass();
	D3DClass(const D3DClass&) = delete;
	D3DClass& operator=(const D3DClass&) = delete;
	~D3DClass();

	bool Initialize(const EngineOptions& options, HWND hwnd);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	void OnResize(const EngineOptions& options);

	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetDeviceContext();
	IDXGISwapChain*			GetSwapChain();

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	Microsoft::WRL::ComPtr<IDXGIFactory>				m_factory;

	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_deviceContext;	
};
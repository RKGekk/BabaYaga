#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include <list>
#include <map>

#include "EngineOptions.h"

class BaseEngineLogic;
class SceneNode;
class MatrixStack;
class ISceneNode;
class IEventData;
class Actor;
class CameraNode;
class FreeCameraNode;
class LightManager;
class ShaderResource;

class SceneTree {
protected:

	std::shared_ptr<SceneNode>							m_Root;
	std::shared_ptr<SceneNode>							m_Offscreen;
	std::shared_ptr<FreeCameraNode>					 	m_Camera;
	std::shared_ptr<LightManager>						m_LightManager;

	std::shared_ptr<MatrixStack>						m_MatrixStack;
	std::map<unsigned int, std::shared_ptr<ISceneNode>>	m_ActorMap;
	std::shared_ptr<BaseEngineLogic>					m_GameLogic;

	std::unique_ptr<ShaderResource>						m_renderTarget;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_offscreenTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_offscreenShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_offscreenRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_offscreenUnorderedAccessView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		m_depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		m_rasterState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_screenQuadVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_screenQuadIB;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayoutsPost;
	const D3D11_INPUT_ELEMENT_DESC m_inputLayoutDescPost[2] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

public:
	SceneTree(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~SceneTree();

	HRESULT OnRender(ID3D11DeviceContext* deviceContext);
	HRESULT OnRestore(const EngineOptions& options, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	HRESULT OnUpdate(const float deltaMilliseconds, ID3D11DeviceContext* deviceContext);

	std::shared_ptr<ISceneNode> FindSceneNode(unsigned int actorId);
	std::shared_ptr<Actor> FindActor(unsigned int actorId);

	bool AddChild(unsigned int actorId, std::shared_ptr<ISceneNode> kid);
	bool RemoveChild(unsigned int actorId);

	// event delegates
	void NewRenderComponentDelegate(std::shared_ptr<IEventData> pEventData);
	void ModifiedRenderComponentDelegate(std::shared_ptr<IEventData> pEventData);
	void DestroyActorDelegate(std::shared_ptr<IEventData> pEventData);
	void MoveActorDelegate(std::shared_ptr<IEventData> pEventData);

	void SetCamera(std::shared_ptr<FreeCameraNode> camera);
	const std::shared_ptr<FreeCameraNode> GetCamera() const;

	void PushAndSetMatrix(const DirectX::XMFLOAT4X4& toWorld);
	void PopMatrix();

	const DirectX::XMFLOAT4X4 GetTopMatrix();
	const DirectX::XMFLOAT4X4 GetTopMatrixT();

	std::shared_ptr<LightManager> GetLightManager();
};
#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <list>
#include <map>

class BaseEngineLogic;
class SceneNode;
class MatrixStack;
class ISceneNode;
class IEventData;
class Actor;
class CameraNode;
class FreeCameraNode;
class LightManager;

class SceneTree {
protected:
	std::shared_ptr<SceneNode>							m_Root;
	std::shared_ptr<FreeCameraNode>					 	m_Camera;
	std::shared_ptr<LightManager>						m_LightManager;

	std::shared_ptr<MatrixStack>						m_MatrixStack;
	std::map<unsigned int, std::shared_ptr<ISceneNode>>	m_ActorMap;
	std::shared_ptr<BaseEngineLogic>					m_GameLogic;

public:
	SceneTree();
	virtual ~SceneTree();

	HRESULT OnRender(ID3D11DeviceContext* deviceContext);
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
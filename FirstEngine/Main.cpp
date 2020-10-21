#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "gdiplus.lib" )

#include <iostream>
#include <memory>

#include "SystemClass.h"
#include <crtdbg.h>

#include "FastDelegate.h"
#include "FastDelegateBind.h"

#include "Actor.h"
#include "ActorComponent.h"
#include "TransformComponent.h"
#include "ActorFactory.h"

#include "BaseEventData.h"
#include "IEventManager.h"
#include "EventManager.h"
#include "EvtData_New_Actor.h"

#include "BaseEngineLogic.h"

#include "ProcessManager.h"
#include "CountProcess.h"

int main() {

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	std::unique_ptr<SystemClass> System(new SystemClass);
	if (System->Initialize()) {
		System->Run();
	}
	// Shutdown and release the system object.
	System->Shutdown();

	//BaseEngineLogic gl;

	//EventManager* m_pEventManager = new EventManager("GameCodeApp Event Mgr", true);

	//IEventManager::Get()->VAddListener(fastdelegate::MakeDelegate(&gl, &BaseEngineLogic::RequestNewActorDelegate), EvtData_New_Actor::sk_EventType);

	//ActorFactory actorFactory;
	//std::shared_ptr<Actor> actor = actorFactory.CreateActor("Teapot", nullptr, nullptr, 0);
	//std::weak_ptr<ActorComponent> actorComponent = actor->GetComponent<TransformComponent>("TransformComponent");

	//std::shared_ptr<EvtData_New_Actor> pNewActorEvent(new EvtData_New_Actor(actor->GetId()));
	//IEventManager::Get()->VQueueEvent(pNewActorEvent);


	//IEventManager::Get()->VUpdate(20);

	//std::shared_ptr<ProcessManager> m_pProcessManager(new ProcessManager);
	//std::shared_ptr<CountProcess> m_pCountProcess1(new CountProcess);
	//std::shared_ptr<CountProcess> m_pCountProcess2(new CountProcess);
	//m_pCountProcess1->AttachChild(m_pCountProcess2);
	//m_pProcessManager->AttachProcess(m_pCountProcess1);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);
	//m_pProcessManager->UpdateProcesses(10);

//	std::cin.get();
	
	return 0;
}
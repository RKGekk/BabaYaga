#include "IEventManager.h"

IEventManager* g_pEventMgr = NULL;
GenericObjectFactory<IEventData, EventTypeId> g_eventFactory;

IEventManager* IEventManager::Get() {
	return g_pEventMgr;
}

IEventDataPtr IEventManager::Create(EventTypeId eventType) {
	return IEventDataPtr(g_eventFactory.Create(eventType));
}

IEventManager::IEventManager(const char* pName, bool setAsGlobal) {
	if (setAsGlobal) {
		if (g_pEventMgr) {
			delete g_pEventMgr;
		}

		g_pEventMgr = this;
	}
}

IEventManager::~IEventManager() {
	if (g_pEventMgr == this)
		g_pEventMgr = NULL;
}
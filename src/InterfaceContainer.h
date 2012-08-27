#ifndef __INCLUDE_INTERFACECONTAINER_H__
#define __INCLUDE_INTERFACECONTAINER_H__

#include "HL2SDK.h"
#include "sdk/smsdk_ext.h"

class InterfaceContainer {
public:
	InterfaceContainer();

	ICvar *CvarInstance;
	IUniformRandomStream *UniformRandomStreamInstance;
	IServerPluginHelpers *ServerPluginsHelperInstance;
	IGameEventManager2 *GameEventManagerInstance;
	IEngineSound *EngineSoundInstance;
	IBaseFileSystem *BaseFileSystemInstance;
	IServerGameClients *ServerGameClientsInstance;
	IServerGameEnts *ServerGameEntsInstance;
	SourceHook::CallClass<IServerGameDLL> *ServerGameDLLCallClass;
	CGlobalVars *GlobalVarsInstance;
};

extern InterfaceContainer g_Interfaces;

#endif
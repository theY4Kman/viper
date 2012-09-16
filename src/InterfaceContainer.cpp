#include "InterfaceContainer.h"

InterfaceContainer g_Interfaces;

InterfaceContainer::InterfaceContainer() {
	CvarInstance = NULL;
	UniformRandomStreamInstance = NULL;
	ServerPluginsHelperInstance = NULL;
	GameEventManagerInstance = NULL;
	EngineSoundInstance = NULL;
	BaseFileSystemInstance = NULL;
	ServerGameClientsInstance = NULL;
	ServerGameEntsInstance = NULL;
	ServerGameDLLCallClass = NULL;
	GlobalVarsInstance = NULL;
	GameConfigInstance = NULL;
	BinToolsInstance = NULL;
	SharedEdictChangeInfoInstance = NULL;
	ServerPluginCallbacksInstance = NULL;
	RootConsoleCommandInstance = NULL;
	NetworkStringTableContainerInstance = NULL;
	EngineTraceInstance = NULL;
	VoiceServerInstance = NULL;

#if SOURCE_ENGINE >= SE_ORANGEBOX
	ServerToolsInstance = NULL;
#endif
	SDKToolsInstance = NULL;
}
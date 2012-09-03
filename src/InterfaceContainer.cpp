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
	ServerGameDLLCallClass = NULL;
	GlobalVarsInstance = NULL;
	GameConfigInstance = NULL;
	BinToolsInstance = NULL;
	SharedEdictChangeInfoInstance = NULL;
	ServerPluginCallbacksInstance = NULL;
	RootConsoleCommandInstance = NULL;
}
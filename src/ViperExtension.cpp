/**
 * =============================================================================
 * Viper
 * Copyright (C) 2012 Anthony "PimpinJuice" Iacono
 * Copyright (C) 2008-2012 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file Viper.cpp
 * @brief Contains the implementation of the SM/MMS extension
 */

#include "ViperExtension.h"
#include "Constants.h"
#include "Util.h"
#include "StdIoRedirect.h"
#include <boost/filesystem.hpp>
#include "SourcemodModule.h"
#include "BitBufModule.h"
#include "HalflifeModule.h"
#include "DatatypesModule.h"
#include "EntityModule.h"
#include "SysHooks.h"
#include "ForwardsModule.h"
#include "ClientsModule.h"
#include "EventsModule.h"
#include "UserMessagesModule.h"
#include "ConsoleModule.h"
#include "InterfaceContainer.h"

namespace py = boost::python;

ViperExtension g_ViperExtension;
SMEXT_LINK(&g_ViperExtension);

SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, false, bool);

ViperExtension::ViperExtension() {
}

ViperExtension::~ViperExtension() {
	delete PluginManagerInstance;
}

void ViperExtension::InitializePython() {
	char pythonPath[PLATFORM_MAX_PATH];

	g_pSM->BuildPath(SourceMod::Path_SM, pythonPath, sizeof(pythonPath),
		"extensions/viper/");

	PythonHome = pythonPath;

	Py_SetPythonHome(pythonPath);

	PyImport_AppendInittab("Sourcemod", initSourcemod);
	PyImport_AppendInittab("BitBuf", initBitBuf);
	PyImport_AppendInittab("Halflife", initHalflife);
	PyImport_AppendInittab("Datatypes", initDatatypes);
	PyImport_AppendInittab("Entity", initEntity);
	PyImport_AppendInittab("Forwards", initForwards);
	PyImport_AppendInittab("Clients", initClients);
	PyImport_AppendInittab("Events", initEvents);
	PyImport_AppendInittab("UserMessages", initUserMessages);
	PyImport_AppendInittab("Console", initConsole);

	Py_Initialize();

	if(Py_IsInitialized() != 0) {
		return;
	}

	boost::throw_exception(std::exception(
		(std::string("Unable to initialize python at home directory: ") +
		std::string(PythonHome)).c_str()));
}

void ViperExtension::InitializePluginManager() {
	char pluginsDirectory[PLATFORM_MAX_PATH];

	g_pSM->BuildPath(SourceMod::Path_SM, pluginsDirectory, sizeof(pluginsDirectory),
		"plugins/");

	PluginsDirectory = pluginsDirectory;

	PluginManagerInstance = new ViperPluginManager(PythonHome);
}

bool ViperExtension::SDK_OnLoad(char *error, size_t maxlength, bool late) {
	// We need bintools for the EntityModule
	sharesys->AddDependency(myself, "bintools.ext", true, true);

	g_Interfaces.SharedEdictChangeInfoInstance = g_pSharedChangeInfo = engine->GetSharedEdictChangeInfo();

	return true;
}

void ViperExtension::SDK_OnUnload() {
	Py_Finalize();

	SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameFrame, gamedll, this, &ViperExtension::OnGameFrame, false);

	destroySourcemod();
	destroyBitBuf();
	destroyHalflife();
	destroyDatatypes();
	destroyEntity();
	destroyForwards();
	destroyClients();
	destroyEvents();
	destroyUserMessages();
	destroyConsole();
}

void ViperExtension::OnGameFrame(bool simulating) {
	console__GameFrame(simulating);
	forwards__GameFrame(simulating);
	clients__GameFrame(simulating);
}

void ViperExtension::SDK_OnAllLoaded() {
	g_Interfaces.ServerGameDLLCallClass = SH_GET_CALLCLASS(gamedll);

	char configError[1024];

	if(!gameconfs->LoadGameConfigFile("viper.games", &g_Interfaces.GameConfigInstance, configError, sizeof(configError))) {
		g_SMAPI->ConPrintf("%s", "Unable to load signatures and offsets file for viper (viper.games.txt)");
		return;
	}

	SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, gamedll, this, &ViperExtension::OnGameFrame, false);

	g_Interfaces.ServerPluginCallbacksInstance = g_SMAPI->GetVSPInfo(NULL);

	try {
		InitializePython();
		InitializePluginManager();

		PluginManagerInstance->LoadPluginsInDirectory(PluginsDirectory);
	}
	catch(std::exception e) {
		g_SMAPI->ConPrintf("%s", e.what());
	}
}

bool ViperExtension::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen,
	bool late) {
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.CvarInstance,
		ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.UniformRandomStreamInstance,
		IUniformRandomStream, VENGINE_SERVER_RANDOM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.ServerPluginsHelperInstance,
		IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.GameEventManagerInstance,
		IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.EngineSoundInstance,
		IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_Interfaces.BaseFileSystemInstance,
		IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_Interfaces.ServerGameClientsInstance,
		IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_ANY(GetServerFactory, g_Interfaces.ServerGameEntsInstance,
		IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);

	g_Interfaces.GlobalVarsInstance = ismm->GetCGlobals();
	g_pCVar = icvar = g_Interfaces.CvarInstance;
	
	return true;
}
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

#include "Extension.h"
#include "Constants.h"
#include "Util.h"
#include "Modules/Sys/StdIoRedirect.h"
#include <boost/filesystem.hpp>
#include "Modules/Sourcemod/SourcemodModule.h"

namespace py = boost::python;
namespace sys = Viper::Modules::Sys;

Viper::Extension g_Extension;
SMEXT_LINK(&g_Extension);

namespace Viper {
	Extension::Extension() {
		CvarInterface = NULL;
		UniformRandomStreamInterface = NULL;
		ServerPluginsHelperInterface = NULL;
		GameEventManagerInterface = NULL;
		EngineSoundInstance = NULL;
		BaseFileSystemInstance = NULL;
		ServerGameClientsInstance = NULL;
	}

	Extension::~Extension() {
		delete PluginManagerInstance;
	}

	void Extension::InitializePython() {
		char pythonPath[PLATFORM_MAX_PATH];

		g_pSM->BuildPath(SourceMod::Path_SM, pythonPath, sizeof(pythonPath),
			"extensions/viper/");

		PythonHome = pythonPath;

		Py_SetPythonHome(pythonPath);

		Py_Initialize();

		if(Py_IsInitialized() != 0) {
			return;
		}

		boost::throw_exception(std::exception(
			(std::string("Unable to initialize python at home directory: ") +
			std::string(PythonHome)).c_str()));
	}

	void Extension::InstallViperTypes() {
		py::object mainModule = py::import("__main__");
		py::object mainNamespace = mainModule.attr("__dict__");

		// Hooking Stdout/err
		mainNamespace["StdIoRedirect"] = py::class_
			<sys::StdIoRedirect>("StdIoRedirect", py::init<>())
			.def("write", &sys::StdIoRedirect::Write)
			.def("Flush", &sys::StdIoRedirect::Flush);

		sys::StdIoRedirect stdIoRedirector;

		py::object sysModule = py::import("sys");

		sysModule.attr("stderr") = stdIoRedirector;
		sysModule.attr("stdout") = stdIoRedirector;
	}

	void Extension::InitializePluginManager() {
		char pluginsDirectory[PLATFORM_MAX_PATH];

		g_pSM->BuildPath(SourceMod::Path_SM, pluginsDirectory, sizeof(pluginsDirectory),
			"plugins/");

		PluginsDirectory = pluginsDirectory;

		PluginManagerInstance = new PluginManager(PythonHome);
	}

	bool Extension::SDK_OnLoad(char *error, size_t maxlength, bool late) {
		GameDLLPatch = SH_GET_CALLCLASS(gamedll);

		try {
			InitializePython();
			InstallViperTypes();
			InitializePluginManager();
		
			PluginManagerInstance->LoadPluginsInDirectory(PluginsDirectory);
		}
		catch(std::exception e) {
			UTIL_Format(error, maxlength, "%s", e.what());
			return false;
		}

		return true;
	}

	void Extension::SDK_OnUnload() {
		Py_Finalize();
	}

	void Extension::SDK_OnAllLoaded() {

	}

	bool Extension::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen,
		bool late) {
		GET_V_IFACE_CURRENT(GetEngineFactory, CvarInterface,
			ICvar, CVAR_INTERFACE_VERSION);
		GET_V_IFACE_CURRENT(GetEngineFactory, UniformRandomStreamInterface,
			IUniformRandomStream, VENGINE_SERVER_RANDOM_INTERFACE_VERSION);
		GET_V_IFACE_CURRENT(GetEngineFactory, ServerPluginsHelperInterface,
			IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
		GET_V_IFACE_CURRENT(GetEngineFactory, GameEventManagerInterface,
			IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
		GET_V_IFACE_CURRENT(GetEngineFactory, EngineSoundInstance,
			IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION);
		GET_V_IFACE_CURRENT(GetEngineFactory, BaseFileSystemInstance,
			IFileSystem, FILESYSTEM_INTERFACE_VERSION);
		GET_V_IFACE_ANY(GetServerFactory, ServerGameClientsInstance,
			IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);

		return true;
	}

	SourceHook::CallClass<IServerGameDLL> *Extension::GetGameDLLPatch() {
		return GameDLLPatch;
	}
}
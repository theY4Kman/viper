/**
 * =============================================================================
 * Viper
 * Copyright (C) 2012 PimpinJuice
 * Copyright (C) 2007-2012 Zach "theY4Kman" Kanzler
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

#ifndef _INCLUDE_Extension_H_
#define _INCLUDE_Extension_H_

#include "HL2SDK.h"
#include "STL.h"
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include "sdk/smsdk_ext.h"
#include "PluginManager.h"

namespace Viper {
	class Extension : public SDKExtension {
	public:
		Extension();
		~Extension();

		void InitializePython();
		void InitializePluginManager();

		SourceHook::CallClass<IServerGameDLL> *GetGameDLLPatch();
		CGlobalVars *GetGlobals();

	public:
		virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
		virtual void SDK_OnUnload();
		virtual void SDK_OnAllLoaded();

	public:
		virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);

	private:
		// This is available after InitializePython()
		std::string PythonHome;

		// This is available after InitializePluginManager()
		std::string PluginsDirectory;

		// Also available after InitializePluginManager()
		PluginManager *PluginManagerInstance;

		ICvar *CvarInterface;
		IUniformRandomStream *UniformRandomStreamInterface;
		IServerPluginHelpers *ServerPluginsHelperInterface;
		IGameEventManager2 *GameEventManagerInterface;
		IEngineSound *EngineSoundInstance;
		IBaseFileSystem *BaseFileSystemInstance;
		IServerGameClients *ServerGameClientsInstance;

		SourceHook::CallClass<IServerGameDLL> *GameDLLPatch;

		CGlobalVars *GlobalsInstance;
	};
}

extern Viper::Extension g_Extension;

#endif /* _INCLUDE_Extension_H_ */
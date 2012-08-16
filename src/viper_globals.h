/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2011 Zach "theY4Kman" Kanzler
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
 * @file viper_globals.h
 * @brief Contains utility functions and various definitions required in many files
 */

#ifndef _INCLUDE_VIPER_GLOBALS_H_
#define _INCLUDE_VIPER_GLOBALS_H_

/* Jesus, sawce, this file is horrendous. Clean up your room. */

#if SOURCE_ENGINE >= SE_LEFT4DEAD
#define gpGlobals g_SMAPI->GetCGlobals()
#define pEdicts baseEdict
#endif

#include <Python.h>
#if SOURCE_ENGINE >= SE_LEFT4DEAD
#   include "convar_sm_l4d.h"
#elif SOURCE_ENGINE >= SE_ORANGEBOX
#   include "convar_sm_ob.h"
#else
#   include "convar_sm.h"
#endif

// Both winnt.h and tier0/commonmacros.h define this, so we'll let Valve win the battle
#undef ARRAYSIZE

#include "sdk/smsdk_ext.h"
#include <random.h>
#include <eiface.h>
#include <IEngineSound.h>
#include <icvar.h>
#include <IRootConsoleMenu.h>
#include <INativeInvoker.h>
#include <IGameConfigs.h>
#include <igameevents.h>
#include <filesystem.h>
#include "python/py_datatypes.h"
#include "python_delay_data.h"

#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

extern ICvar *icvar;

using namespace Viper::Python;

#if SOURCE_ENGINE >= SE_ORANGEBOX
extern ICvar *g_pCVar;
#endif

/* Interface declaration */
extern IUniformRandomStream *g_pRandom;
extern SourceMod::IRootConsole *g_pMenu;
extern SourceHook::CallClass<IServerGameDLL> *g_pGameDLLPatch;
extern IServerPluginHelpers *g_pServerPluginHelpers;
extern IServerGameClients *g_pServerClients;
extern IGameEventManager2 *g_pGameEvents;
extern IEngineSound *enginesound;
extern SourceMod::IGameConfig *g_pGameConf;
extern IFileSystem *g_pBaseFilesystem;

extern SourceMod::INativeInterface *g_pNInvoke;

extern PyThreadState *g_pGlobalThreadState;
extern PyObject *g_pViperException;
extern PyObject *g_pViperExtensionException;

namespace Viper {
	#if SOURCE_ENGINE < SE_ORANGEBOX
	#   define GetEngineFactory engineFactory
	#endif

	#define IS_STR_FILLED(str) ((str) != NULL && (str)[0] != '\0')

	#ifndef PyModule_AddIntMacro
	#   define PyModule_AddIntMacro(module, constant) PyModule_AddIntConstant(module, #constant, constant)
	#endif

	/* Retrieve the plug-in of the current thread state. */
	#define GET_THREAD_PLUGIN() IViperPlugin *pPlugin; { \
		PyThreadState *tstate = PyThreadState_Get(); \
		assert(tstate != NULL); \
		pPlugin = g_VPlugins.GetPluginOfInterpreterState(tstate->interp); \
		if (pPlugin == NULL) \
		{ \
			PyErr_SetString(g_pViperException, "Error retrieving current plug-in"); \
			return 0; \
		} \
	}

	#define SERVER_CALL(func) SH_CALL(g_pGameDLLPatch, &IServerGameDLL::func)

	typedef PyObject PyFunction;

	PyObject *CreatePyVector(float x=0.0f, float y=0.0f, float z=0.0f);
	PyObject *CreatePyVector(const QAngle &ang);
	PyObject *CreatePyVector(const Vector &vec);

	PyObject *CreatePyColor(Color color);
	PyObject *CreatePyColor(int r=0, int g=0, int b=0, int a=0);

	Color *ColorFromPyColor(datatypes__Color *color);
	

	/* Utility functions from various authors */
	/* From sm_stringutils.cpp */
	size_t UTIL_Format(char *buffer, size_t maxlength, char const *fmt, ...);

	/* From sm_stringutils.cpp */
	char *sm_strdup(char const *str);

	/* StrReplace by sslice. I bet he doesn't know :O */
	int StrReplace(char *str, char const *from, char const *to, int maxlen);

	/**
	 * @brief Retrieves the name of the deepest folder in a path
	 */
	char const *GetLastFolderOfPath(char const *path);

	/**
	 * @brief Retrieves all the text past the last slash in a path
	 */
	char const *GetLastOfPath(char const *path);

	/* From sm_stringutils.cpp */
	unsigned int strncopy(char *dest, char const *src, size_t count);

	class ViperGlobalClass
	{
		friend class BaseViper;
	public:
		ViperGlobalClass();
	public:
		/**
		 * @brief Called when Viper is initially loading
		 */
		virtual void OnViperStartup(bool late)
		{
		}

		/**
		 * @brief Called after all global classes have been started up
		 */
		virtual void OnViperAllInitialized()
		{
		}

		/**
		 * @brief Called after all global classes have initialized
		 */
		virtual void OnViperAllInitialized_Post()
		{
		}

		/**
		 * @brief Called when Viper is shutting down
		 */
		virtual void OnViperShutdown()
		{
		}

		/**
		 * @brief Called after Viper is completely shut down
		 */
		virtual void OnViperAllShutdown()
		{
		}

	private:
		ViperGlobalClass *m_pGlobalClassNext;
		static ViperGlobalClass *head;
	};
}

#endif /* _INCLUDE_VIPER_GLOBALS_H_ */


/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
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

#include <Python.h>
#if SOURCE_ENGINE == SE_LEFT4DEAD
#   include "convar_sm_l4d.h"
#elif SOURCE_ENGINE == SE_ORANGEBOX
#   include "convar_sm_ob.h"
#else
#   include "convar_sm.h"
#endif
#include "sdk/smsdk_ext.h"
#include <random.h>
#include <eiface.h>
#include <IEngineSound.h>
#include <icvar.h>
#include <IRootConsoleMenu.h>
#include <IGameConfigs.h>
#include <igameevents.h>
#include "python/py_datatypes.h"
#include "python_delay_data.h"

#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

/* Interface declaration */
extern IUniformRandomStream *g_pRandom;
extern SourceMod::IRootConsole *g_pMenu;
extern ICvar *icvar;
extern SourceHook::CallClass<IServerGameDLL> *g_pGameDLLPatch;
extern IServerPluginHelpers *g_pServerPluginHelpers;
extern IGameEventManager2 *gameevents;
extern IEngineSound *enginesound;
extern SourceMod::IGameConfig *g_pGameConf;

#if SOURCE_ENGINE >= SE_ORANGEBOX
extern ICvar *g_pCVar;
#endif

extern PyThreadState *g_pGlobalThreadState;
extern PyObject *g_pViperException;

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
        PyErr_SetString(g_pViperException, "Error retrieving current plug-in "); \
        return NULL; \
    } \
}

#define SERVER_CALL(func) SH_CALL(g_pGameDLLPatch, &IServerGameDLL::func)

typedef PyObject PyFunction;

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

/* Utility functions for PyObject data types */
PyObject *CreatePyVector(float x=0.0f, float y=0.0f, float z=0.0f);
PyObject *CreatePyColor(int r=0, int g=0, int b=0, int a=0);
inline PyObject *CreatePyVector(const Vector vec)
{
    return CreatePyVector(vec.x, vec.y, vec.z);
}
inline PyObject *CreatePyVector(const QAngle ang)
{
    return CreatePyVector(ang.x, ang.y, ang.z);
}

inline Vector *VectorFromPyVector(datatypes__Vector *vec)
{
    return new Vector(vec->x, vec->y, vec->z);
}

inline PyObject *CreatePyColor(Color color)
{
    return CreatePyColor(color[0], color[1], color[2], color[3]);
}

inline Color *ColorFromPyColor(datatypes__Color *color)
{
    return new Color(color->r, color->g, color->b, color->a);
}

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

#endif /* _INCLUDE_VIPER_GLOBALS_H_ */


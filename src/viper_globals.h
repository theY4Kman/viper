/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
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

#include <Python.h>
#include "sdk/smsdk_ext.h"
//#include "viper.h"
#include <random.h>
#include <icvar.h>
#include <IRootConsoleMenu.h>

/* Interface declaration */
extern IUniformRandomStream *g_pRandom;
extern SourceMod::IRootConsole *g_pMenu;
extern ICvar *icvar;
extern SourceHook::CallClass<IServerGameDLL> *g_pGameDLLPatch;
extern IServerPluginHelpers *g_pServerPluginHelpers;

#ifdef ORANGEBOX_BUILD
extern ICvar *g_pCVar;
#endif

#ifdef WIN32
/* When delay loading in Windows, data variables are unavailable, so we must
 * define them ourselves.
 */
#undef Py_None
#undef _Py_TrueStruct;
#undef _Py_ZeroStruct;
extern PyObject *Py_None;
extern PyObject *_Py_TrueStruct;
extern PyObject *_Py_ZeroStruct;
#endif

extern PyThreadState *g_pGlobalThreadState;
extern PyObject *g_pViperException;

#ifndef ORANGEBOX_BUILD
#define GetEngineFactory engineFactory
#endif

#define IS_STR_FILLED(str) ((str) != NULL && (str)[0] != '\0')

#ifndef PyModule_AddIntMacro
#define PyModule_AddIntMacro(module, constant) PyModule_AddIntConstant(module, #constant, constant)
#endif

/* Retrieve the plug-in of the current thread state. */
#define GET_THREAD_PLUGIN() IViperPlugin *pPlugin; { \
    PyObject *thread_dict = PyThreadState_GetDict(); \
    PyObject *pyPlugin = PyDict_GetItemString(thread_dict, "viper_cplugin"); \
    \
    if (pyPlugin == NULL \
        || (pPlugin = (IViperPlugin*)PyCObject_AsVoidPtr(pyPlugin)) == NULL) \
    { \
        PyErr_SetString(g_pViperException, "The current thread state has no " \
            "plug-in associated"); \
        return NULL; \
    } \
}

typedef PyObject PyFunction;

/* Utility functions from various authors */
/* From sm_stringutils.cpp */
extern size_t UTIL_Format(char *buffer, size_t maxlength, char const *fmt, ...);

/* From sm_stringutils.cpp */
extern char *sm_strdup(char const *str);

/* StrReplace by sslice. I bet he doesn't know :O */
extern int StrReplace(char *str, char const *from, char const *to, int maxlen);

/**
 * @brief Retrieves the name of the deepest folder in a path
 */
extern char const *ParseNiceName(char const *path);

/* From sm_stringutils.cpp */
extern unsigned int strncopy(char *dest, char const *src, size_t count);

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


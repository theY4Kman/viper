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
 * @file extension.cpp
 * @brief Contains the implementation of the SM/MMS extension
 */

#include "extension.h"
#include <random.h>
#include "python/init.h"
#include "viper.h"

ViperExtension g_ViperExt;
SMEXT_LINK(&g_ViperExt);

ICvar *icvar   = NULL;
#ifdef ORANGEBOX_BUILD
ICvar *g_pCVar = NULL;
#endif
IServerPluginHelpers *g_pServerPluginHelpers = NULL;
IUniformRandomStream *g_pRandom = NULL;
IGameEventManager2 *gameevents = NULL;

#ifdef WIN32
PyObject *Py_None = NULL;
PyObject *_Py_TrueStruct = NULL;
PyObject *_Py_ZeroStruct = NULL;
#endif

PyThreadState *g_pGlobalThreadState = NULL;

void
InitializePython(void)
{
    /* Initialize the python interpreter, passing 0 to skip registration of
     * Python's default signal handlers (like SIGINT)
     */
    Py_InitializeEx(0);
    
    /* This will be removed until threads are determined to be useful. */  
    /* PyEval_InitThreads(); */
    
    g_pGlobalThreadState = PyThreadState_Get();
    
    /* Call initsourcemod so that it is loaded into the main thread state.
     * This allows ES users to access the sourcemod module.
     */
    initsourcemod();
}

bool
ViperExtension::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
#ifdef WIN32
    char libpath[PLATFORM_MAX_PATH];
    /* Sets the DLL search path in Windows */
    
    g_pSM->BuildPath(SourceMod::Path_SM, libpath, sizeof(libpath),
        "extensions/viper/lib/plat-win/");
    SetDllDirectory(libpath);
    
    HMODULE python25_DLL = NULL;
    if (!(python25_DLL = GetModuleHandle(_T("python25.dll"))))
        return false;
    
    Py_None = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_NoneStruct");
    _Py_TrueStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_TrueStruct");
    _Py_ZeroStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_ZeroStruct");
#else
    /* We must load in the binary to allow access to it.
     * Thanks to your-name-here for that bit of info!
     */
    if (dlopen("libpython2.5.so.1.0", RTLD_NOW) == NULL)
        return false;
#endif
    
    InitializePython();
    
    return g_Viper.OnViperLoad(error, maxlength, late);
}

void
ViperExtension::SDK_OnUnload()
{
    g_Viper.OnViperUnload();
    
    PyThreadState_Swap(g_pGlobalThreadState);
    
    /* Reset sys.stdout */
    PyObject *sourcemod = PyImport_AddModule("sourcemod");
    PySys_SetObject("stdout", PyObject_GetAttrString(sourcemod, "stdout"));
    
    Py_Finalize();
}

void
ViperExtension::SDK_OnAllLoaded()
{
    // TODO
}

bool
ViperExtension::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen,
                                  bool late)
{
    GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pRandom, IUniformRandomStream,
        VENGINE_SERVER_RANDOM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pServerPluginHelpers,
        IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
    GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2,
        INTERFACEVERSION_GAMEEVENTSMANAGER2);
    
#ifdef ORANGEBOX_BUILD
    g_pCVar = icvar;
#endif

    return true;
}


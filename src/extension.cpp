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

#ifdef WIN32
PyObject *Py_None = NULL;
PyObject *_Py_TrueStruct = NULL;
PyObject *_Py_ZeroStruct = NULL;
#endif

PyThreadState *g_pGlobalThreadState = NULL;

void
InitializePython(void)
{
    /* This will cause Python to load, but not import, the module "sourcemod"
     * for every plugin that gets loaded.
     */
    PyImport_AppendInittab("sourcemod", initsourcemod);
    
    /* Change the path Python looks at for its core modules */
    char libpath[PLATFORM_MAX_PATH];
    g_pSM->BuildPath(SourceMod::Path_SM, libpath, sizeof(libpath), "extensions/viper/lib/");
    
    /* Initialize the python interpreter */
    Py_Initialize();
    
    PySys_SetPath(libpath);
    
    PyEval_InitThreads();
    g_pGlobalThreadState = PyThreadState_Get();
}

bool
ViperExtension::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
#ifdef WIN32
    char libpath[PLATFORM_MAX_PATH];
    /* Sets the DLL search path in Windows */
    
    g_pSM->BuildPath(SourceMod::Path_SM, libpath, sizeof(libpath), "extensions/viper/lib/plat-win/");
    SetDllDirectory(libpath);
    
    HMODULE python25_DLL = NULL;
    if (!(python25_DLL = GetModuleHandle(_T("python25.dll"))))
        return false;
    
    Py_None = (PyObject*)GetProcAddress(g_python25_DLL, "Py_None");
    _Py_TrueStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_TrueStruct");
    _Py_ZeroStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_ZeroStruct");
#else
    /* We must load in the binary to allow access to it.
     * Thanks to your-name-here for that bit of info!
     */
    if (dlopen("libpython2.5.so", RTLD_NOW) == NULL)
        return false;
#endif
    
    InitializePython();
    
    return g_Viper.OnViperLoad(error, maxlength, late);
}

void
ViperExtension::SDK_OnUnload()
{
    PyThreadState_Swap(g_pGlobalThreadState);
    Py_Finalize();
}

void
ViperExtension::SDK_OnAllLoaded()
{
    // TODO
}

bool
ViperExtension::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pRandom, IUniformRandomStream,
        VENGINE_SERVER_RANDOM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pServerPluginHelpers,
        IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);

#ifdef ORANGEBOX_BUILD
    g_pCVar = icvar;
#endif

    return true;
}


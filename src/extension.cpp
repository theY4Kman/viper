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
#include <metamod_wrappers.h>
#include <random.h>
#include "python/init.h"
#include "python/py_entity.h"
#include "viper.h"
#include <IForwardSys.h>

#if defined __linux__
/* I wish sawce would get out of GCC, so it wouldn't be so horrible. btw, hax */
extern "C" int __cxa_guard_acquire(long long int *) { return 0; }
extern "C" void __cxa_guard_release(long long int *) { return; }
#endif

ViperExtension g_ViperExt;
SMEXT_LINK(&g_ViperExt);

ICvar *icvar   = NULL;
#if SOURCE_ENGINE < SE_ORANGEBOX
ICvar *g_pCVar = NULL;
#endif
IServerPluginHelpers *g_pServerPluginHelpers = NULL;
IUniformRandomStream *g_pRandom = NULL;
IGameEventManager2 *gameevents = NULL;
IEngineSound *enginesound = NULL;

SourceMod::IForward *g_pSMOnBanIdentity = NULL;
SourceMod::IForward *g_pSMOnBanClient = NULL;

#ifdef WIN32
PyObject *Py_None = NULL;
PyObject *_Py_TrueStruct = NULL;
PyObject *_Py_ZeroStruct = NULL;
#endif

PyThreadState *g_pGlobalThreadState = NULL;

void
InitializePython(void)
{
    if (!Py_IsInitialized())
    {
        /* Initialize the python interpreter, passing 0 to skip registration of
         * Python's default signal handlers (like SIGINT)
         */
        Py_InitializeEx(0);
    }
    
    /* Save the thread state -- this may be the thread state ES is running in */
    g_pGlobalThreadState = PyThreadState_Get();
    
    /* Call initsourcemod so that it is loaded into the main thread state.
     * This allows ES users to access the sourcemod module.
     * XXX: Does this actually work? TEST!
     */
    //initsourcemod();
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
    
    HMODULE python25_DLL = GetModuleHandle(_T("python25.dll"));
    if (python25_DLL == NULL)
    {
        strncpy(error, "Unable to load python25.dll", maxlength);
        return false;
    }
    
    Py_None = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_NoneStruct");
    _Py_TrueStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_TrueStruct");
    _Py_ZeroStruct = (PyObject*)GetProcAddress(g_python25_DLL, "_Py_ZeroStruct");
    
    g_pSendProxy_EHandleToInt = memutils->FindPattern(g_SMAPI->GetServerFactory(false),
        "\x83\x2A\x2A\x8B\x2A\x2A\x2A\x8B\x2A\x2A\x2A\x33\xC9\x89\x2A\x2A\x2A"
        "\x8B\x2A\x2A\x2A\x89\x2A\x2A\x2A\x8B\x2A\x2A\x2A\x3D\x70\xC1\x13\x22"
        "\xC7\x2A\x2A\x5C\x4C\x44\x22", 41);
    
    if (g_pSendProxy_EHandleToInt == NULL)
    {
        strncpy(error, "Could not find SendProxy_EHandleToInt: entity property type "
                "autodetection would fail without it. Unloading.", maxlength);
        return false;
    }
#else
    /* We must load in the binary to allow access to it.
     * Thanks to your-name-here for that bit of info!
     */
    if (dlopen("libpython2.5.so.1.0", RTLD_NOW) == NULL)
    {
        strncpy(error, "Unable to load libpython2.5.so.1.0", maxlength);
        return false;
    }
    
    Dl_info info;
    if (dladdr((void*)g_SMAPI->GetServerFactory(false), &info) == 0)
    {
        strncpy(error, "Could not find SendProxy_EHandleToInt: entity property type "
                "autodetection would fail without it. Unloading.", maxlength);
        return false;
    }
    
    void *handle = dlopen(info.dli_fname, RTLD_NOW);
    if (handle == NULL)
    {
        strncpy(error, "Could not find SendProxy_EHandleToInt: entity property type "
                "autodetection would fail without it. Unloading.", maxlength);
        return false;
    }
    
    g_pSendProxy_EHandleToInt = dlsym(handle, "_Z22SendProxy_EHandleToIntPK8SendPropPKvS3_P8DVariantii");
    dlclose(handle);
    
    if (g_pSendProxy_EHandleToInt == NULL)
    {
        strncpy(error, "Could not find SendProxy_EHandleToInt: entity property type "
                "autodetection would fail without it. Unloading.", maxlength);
        return false;
    }
#endif
    
    InitializePython();
    
    return g_Viper.OnViperLoad(error, maxlength, late);
}

void
ViperExtension::SDK_OnUnload()
{
    g_Viper.OnViperUnload();
    
    PyThreadState_Swap(g_pGlobalThreadState);
    
    Py_Finalize();
}

void
ViperExtension::SDK_OnAllLoaded()
{
    // TODO: SourceMod natives
    
    g_pSMOnBanIdentity = g_pForwards->FindForward("OnBanIdentity", NULL);
    g_pSMOnBanClient = g_pForwards->FindForward("OnBanClient", NULL);
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
    GET_V_IFACE_CURRENT(GetEngineFactory, enginesound, IEngineSound,
        IENGINESOUND_SERVER_INTERFACE_VERSION);

    
#if SOURCE_ENGINE < SE_ORANGEBOX
    g_pCVar = icvar;
#endif

    return true;
}


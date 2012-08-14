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
 * @file extension.cpp
 * @brief Contains the implementation of the SM/MMS extension
 */

#include "extension.h"
#include "viper_metamod_wrappers.h"
#include <random.h>
#include "python/init.h"
#include "python/py_entity.h"
#include "viper.h"
#include "console.h"
#include <IForwardSys.h>
#include <dt_send.h>

#if defined __linux__
/* I wish sawce would get out of GCC, so it wouldn't be so horrible. btw, hax */
extern "C" int __cxa_guard_acquire(long long int *) { return 0; }
extern "C" void __cxa_guard_release(long long int *) { return; }
#endif

using namespace Viper;
using namespace Viper::Python;

ViperExtension g_ViperExt;
SMEXT_LINK(&g_ViperExt);

ICvar *icvar   = NULL;

#if defined(WIN32) && defined(Py_TRACE_REFS)
	Py_ssize_t *__Py_RefTotal = NULL;
#endif

namespace Viper {
	SourceMod::INativeInterface *ninvoke = NULL;

	#if SOURCE_ENGINE >= SE_ORANGEBOX
	ICvar *g_pCVar = NULL;
	#endif
	IServerPluginHelpers *g_pServerPluginHelpers = NULL;
	IServerGameClients *serverClients = NULL;
	IUniformRandomStream *g_pRandom = NULL;
	IGameEventManager2 *gameevents = NULL;
	IEngineSound *enginesound = NULL;
	IFileSystem *baseFs = NULL;

	SourceMod::IForward *g_pSMOnBanIdentity = NULL;
	SourceMod::IForward *g_pSMOnBanClient = NULL;

	extern ViperConsole g_VConsole;

	namespace Python {
#ifdef WIN32
		PyObject *Py_None = NULL;
		PyObject *Py_True = NULL;
		PyObject *Py_False = NULL;
		PyObject *Py_NotImplemented = NULL;

		PyObject *_PyExc_TypeError = NULL;
		PyObject *_PyExc_IndexError = NULL;
		PyObject *_PyExc_IOError = NULL;
		PyObject *_PyExc_KeyError = NULL;
		PyObject *_PyExc_RuntimeError = NULL;
		PyObject *_PyExc_RuntimeWarning = NULL;
		PyObject *_PyExc_SystemExit = NULL;
		PyObject *_PyExc_StopIteration = NULL;

		PyTypeObject *_PyInt_Type = NULL;
		PyTypeObject *_PyString_Type = NULL;
		PyTypeObject *_PyFloat_Type = NULL;
		PyTypeObject *_PyType_Type = NULL;
		PyTypeObject *_PyLong_Type = NULL;
		PyTypeObject *_PyTuple_Type = NULL;
		PyTypeObject *_PyBool_Type = NULL;
		PyTypeObject *_PyDict_Type = NULL;
		PyTypeObject *_PyList_Type = NULL;
		PyTypeObject *_PyFile_Type = NULL;
#endif
	}

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
		SM_GET_IFACE(NINVOKE, ninvoke);

	#ifdef WIN32
		char libpath[PLATFORM_MAX_PATH];
		/* Sets the DLL search path in Windows */
    
		g_pSM->BuildPath(SourceMod::Path_SM, libpath, sizeof(libpath),
			"extensions/viper/lib/plat-win/");
		SetDllDirectory(libpath);

		//////////////////////////////////
		META_CONPRINTF("bpath: %s", libpath);
    
		HMODULE python25_DLL = LoadLibrary("python25_d.dll"); // lol _d
		if (python25_DLL == NULL)
		{
			LPVOID errorMsg;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &errorMsg,
				0, NULL
			);
        
			size_t written = UTIL_Format(error, maxlength, "Unable to load python25.dll (0x%x): %s", GetLastError(), errorMsg);
			error[written-2] = '\0';
			return false;
		}
    
		Py_None = (PyObject*)GetProcAddress(python25_DLL, "_Py_NoneStruct");
		Py_True = (PyObject*)GetProcAddress(python25_DLL, "_Py_TrueStruct");
		Py_False = (PyObject*)GetProcAddress(python25_DLL, "_Py_ZeroStruct");
		Py_NotImplemented = (PyObject*)GetProcAddress(python25_DLL, "_Py_NotImplementedStruct");
    
		_PyExc_TypeError = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_TypeError"));
		_PyExc_IndexError = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_IndexError"));
		_PyExc_IOError = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_IOError"));
		_PyExc_RuntimeError = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_RuntimeError"));
		_PyExc_KeyError = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_KeyError"));
		_PyExc_RuntimeWarning = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_RuntimeWarning"));
		_PyExc_SystemExit = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_SystemExit"));
		_PyExc_StopIteration = *((PyObject**)GetProcAddress(python25_DLL, "PyExc_StopIteration"));

	#if defined(Py_TRACE_REFS)
		__Py_RefTotal = *((Py_ssize_t**)GetProcAddress(python25_DLL, "_Py_RefTotal"));
	#endif
    
	#else
		/* We must load in the binary to allow access to it.
			* Thanks to your-name-here for that bit of info!
			*/
		if (dlopen("libpython2.5.so.1.0", RTLD_NOW) == NULL)
		{
			strncpy(error, "Unable to load libpython2.5.so.1.0", maxlength);
			return false;
		}

	#endif /* WIN32 */

		SourceMod::sm_sendprop_info_t propinfo;
		bool ret = gamehelpers->FindSendPropInfo("CBaseEntity", "m_hOwnerEntity", &propinfo);
		if (!ret)
		{
			strncpy(error, "Could not fetch SendProp info: entity property type "
					"autodetection would fail without it. Unloading.", maxlength);
		}
    
		g_pSendProxy_EHandleToInt = (void*)propinfo.prop->GetProxyFn();
		if (g_pSendProxy_EHandleToInt == NULL)
		{
			strncpy(error, "Could not find SendProxy_EHandleToInt: entity property type "
					"autodetection would fail without it. Unloading.", maxlength);
			return false;
		}
    
		InitializePython();
    
	#ifdef WIN32
		// The types module is easy and fast, so we'll try to use it first.
		PyObject *types = PyImport_ImportModule("types");
		if (types == NULL)
		{
			// If we can't use it, we have to grab the types from __main__
			PyObject *builtin = PyImport_ImportModule("__builtin__");
        
			Py_XINCREF(_PyType_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "type"));
			Py_XINCREF(_PyInt_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "int"));
			Py_XINCREF(_PyString_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "str"));
			Py_XINCREF(_PyFloat_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "float"));
			Py_XINCREF(_PyLong_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "long"));
			Py_XINCREF(_PyTuple_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "tuple"));
			Py_XINCREF(_PyBool_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "bool"));
			Py_XINCREF(_PyDict_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "dict"));
			Py_XINCREF(_PyList_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "list"));
			Py_XINCREF(_PyFile_Type = (PyTypeObject *)PyObject_GetAttrString(builtin, "file"));
		}
		else
		{
			Py_XINCREF(_PyType_Type = (PyTypeObject *)PyObject_GetAttrString(types, "TypeType"));
			Py_XINCREF(_PyInt_Type = (PyTypeObject *)PyObject_GetAttrString(types, "IntType"));
			Py_XINCREF(_PyString_Type = (PyTypeObject *)PyObject_GetAttrString(types, "StringType"));
			Py_XINCREF(_PyFloat_Type = (PyTypeObject *)PyObject_GetAttrString(types, "FloatType"));
			Py_XINCREF(_PyLong_Type = (PyTypeObject *)PyObject_GetAttrString(types, "LongType"));
			Py_XINCREF(_PyTuple_Type = (PyTypeObject *)PyObject_GetAttrString(types, "TupleType"));
			Py_XINCREF(_PyBool_Type = (PyTypeObject *)PyObject_GetAttrString(types, "BooleanType"));
			Py_XINCREF(_PyDict_Type = (PyTypeObject *)PyObject_GetAttrString(types, "DictType"));
			Py_XINCREF(_PyList_Type = (PyTypeObject *)PyObject_GetAttrString(types, "ListType"));
			Py_XINCREF(_PyFile_Type = (PyTypeObject *)PyObject_GetAttrString(types, "FileType"));
        
			Py_DECREF(types);
		}
    
	#   define CHECK_TYPE_IS_NULL(type) if (_Py##type##_Type == NULL) { \
			strncpy(error, "Unable to initialize Python " #type " type -- cannot run Viper.", \
				maxlength); \
			return false; }
    
		CHECK_TYPE_IS_NULL(Type);
		CHECK_TYPE_IS_NULL(Int);
		CHECK_TYPE_IS_NULL(String);
		CHECK_TYPE_IS_NULL(Float)
		CHECK_TYPE_IS_NULL(Long)
		CHECK_TYPE_IS_NULL(Tuple)
		CHECK_TYPE_IS_NULL(Bool)
		CHECK_TYPE_IS_NULL(Dict)
		CHECK_TYPE_IS_NULL(List);
		CHECK_TYPE_IS_NULL(File);

	#endif
    
		/* For some reason, Python ends up with types Viper has created, but they
			* all have null ob_types. We remedy that by instantiating all the modules
			* with their initxxxxx() functions, which set ob_type. We can't call
			* initsourcemod(), because it corrupts the stack (I don't know why)
			*/
	#   define SetObType(x) Py_XDECREF(init##x());
		SetObType(bitbuf);
		SetObType(clients);
		SetObType(console);
		SetObType(datatypes);
		SetObType(entity);
		SetObType(events);
		SetObType(forwards);
		SetObType(halflife);
		SetObType(keyvalues);
		SetObType(usermessages);
    
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
		GET_V_IFACE_CURRENT(GetEngineFactory, baseFs, IFileSystem,
			FILESYSTEM_INTERFACE_VERSION);
		GET_V_IFACE_ANY(GetServerFactory, serverClients, IServerGameClients,
			INTERFACEVERSION_SERVERGAMECLIENTS);

    
	#if SOURCE_ENGINE >= SE_ORANGEBOX
		g_pCVar = icvar;
	#endif
    
		CONVAR_REGISTER(&g_VConsole);

		return true;
	}
}
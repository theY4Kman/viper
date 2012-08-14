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

#include <Python.h>
#include "ViperPluginSys.h"
#include "ConCmdManager.h"
#include "python/init.h"
#include "viper.h"
#include "console.h"
#include <string.h>

namespace Viper {
	CPluginFunction::~CPluginFunction()
	{
		Py_DECREF(m_pFunc);
	}

	PyObject *
	CPluginFunction::Execute(PyObject *args, PyObject *keywds)
	{
		PyThreadState *_save = PyThreadState_Get();
		PyThreadState_Swap(m_pPlugin->GetThreadState());
    
		assert(PyCallable_Check(m_pFunc));
    
		PyObject *result = PyObject_Call(m_pFunc, args, keywds);
    
		/* Damn it, sawce, stop causing errors in callbacks */
		if (result == NULL && PyErr_Occurred())
			PyErr_Print();
    
		PyThreadState_Swap(_save);
    
		return result;
	}

	IViperPlugin *
	CPluginFunction::GetOwnerPlugin()
	{
		return m_pPlugin;
	}

	PyObject *
	CPluginFunction::GetFunction()
	{
		return m_pFunc;
	}

	ViperResultType
	CPluginFunction::ForwardCallback(IViperForward *fwd, PyObject *result,
									 IViperPluginFunction *fwd_function)
	{
		PyObject *args = PyTuple_Pack(2, result, fwd_function->GetFunction());
    
		PyThreadState *_save = PyThreadState_Get();
		PyThreadState_Swap(m_pPlugin->GetThreadState());
    
		assert(PyCallable_Check(m_pFunc));
    
		PyObject *pyresult = PyObject_CallObject(m_pFunc, args);
    
		/* Damn it, sawce, stop causing errors in callbacks */
		if (pyresult == NULL && PyErr_Occurred())
		{
			PyErr_Print();
        
			// Don't stop executing callbacks because one had an error
			return Pl_Continue;
		}
    
		if (!PyInt_Check(pyresult))
			return Pl_Continue;
    
		int int_res = PyInt_AS_LONG(pyresult);
		if (int_res < Pl_Continue)
			return Pl_Continue;
    
		if (int_res > Pl_Stop)
			return Pl_Stop;
    
		PyThreadState_Swap(_save);
    
		return (ViperResultType)int_res;
	}

	CPluginFunction *
	CPluginFunction::CreatePluginFunction(PyFunction *func, IViperPlugin *pl)
	{
		assert(func != NULL);
		assert(pl != NULL);
    
		CPluginFunction *plfunc = NULL;
    
		PyThreadState *_save = PyThreadState_Get();
		PyThreadState_Swap(pl->GetThreadState());
    
		if (!PyCallable_Check(func))
			goto end;
    
		plfunc = new CPluginFunction;
		plfunc->m_pFunc = func;
		plfunc->m_pPlugin = pl;
    
		Py_INCREF(func);
    
	end:
		PyThreadState_Swap(_save);
    
		return plfunc;
	}


	CPlugin::CPlugin(char const *file)
	{
		m_psError = NULL;
		m_psErrorType = NULL;
		m_pErrorTraceback = NULL;
    
		m_pProps = sm_trie_create();
    
		strncpy(m_sPath, file, sizeof(m_sPath));
		StrReplace(m_sPath, "\\", "/", PLATFORM_MAX_PATH);
    
		m_sFolder = GetLastFolderOfPath(m_sPath);
    
		m_info.name = m_sFolder;
		m_info.author = "";
		m_info.description = "";
		m_info.version = "";
		m_info.url = "";
    
		/* Create a completeley new environment (sub-interpreter) */
		m_pThreadState = Py_NewInterpreter();
		if (m_pThreadState == NULL)
		{
			g_pSM->LogError(myself, "Fatal error in creating Python "
				"sub-interpreter for %s!", m_sFolder);
			g_VPlugins.UnloadPlugin(this);
			return;
		}
    
		m_pInterpState = m_pThreadState->interp;
	}

	CPlugin::~CPlugin()
	{
		sm_trie_destroy(m_pProps);
    
		if (m_psErrorType)
		{
			delete [] m_psErrorType;
			m_psErrorType = NULL;
		}
    
		if (m_psError)
		{
			delete [] m_psError;
			m_psError = NULL;
		}
    
		if (m_pErrorTraceback)
		{
			Py_DECREF(m_pErrorTraceback);
			m_pErrorTraceback = NULL;
		}
    
		/* Destroy the plug-in's sub-interpreter and switch back to main */
		if (m_pThreadState != NULL)
		{
			PyThreadState *_swap = PyThreadState_Get();
			PyThreadState_Swap(m_pThreadState);
        
			if (_swap == m_pThreadState)
				_swap = NULL;
        
			if (GetStatus() == ViperPlugin_Running)
			{
				/* If the plug-in has enabled threads (by using any sort of threading),
				 * we must kill the threads before destroying the interpreter */
				if (PyEval_ThreadsInitialized())
				{
					// Loop through all threads in the current interpreter and raise SystemExit
					PyThreadState *curstate = m_pInterpState->tstate_head;
					while (curstate != NULL && curstate->interp == m_pInterpState)
					{
						PyThreadState_Swap(curstate);
                    
						PyThreadState_SetAsyncExc(curstate->thread_id, _PyExc_SystemExit);
						curstate = curstate->next;
                    
						PyThreadState_Swap(NULL);
					}
                
					/* This is very much frowned upon in Py_EndInterpreter, but this little
					 * hack makes Python happy. I don't know the consequences of this line,
					 * but until I do, I won't feel guilty >_> */
					m_pInterpState->tstate_head = m_pThreadState;
				}
            
				PyThreadState_Swap(m_pThreadState);
				Py_EndInterpreter(m_pThreadState);
			}
			else
			{
				PyThreadState_Swap(_swap);
				PyThreadState_Clear(m_pThreadState);
				PyThreadState_Delete(m_pThreadState);
			}
        
			PyThreadState_Swap(NULL);
		}
	}

	CPlugin *
	CPlugin::CreatePlugin(char const *path, char* error, size_t maxlength)
	{
		CPlugin *pPlugin = new CPlugin(path);
		assert(pPlugin != NULL);

		if (!libsys->PathExists(path))
			pPlugin->m_status = ViperPlugin_BadLoad;

		return pPlugin;
	}

	PyObject *
	InitializePlugin(char const *path)
	{
		/* Clear sys.path and add the plug-in's folder, as well as Python's libs */
		char *path_string = new char[PLATFORM_MAX_PATH];
		char const *endpath = strrchr(path, '/');
		size_t len;
		if (endpath == NULL)
			len = sizeof(path_string);
		else
			len = endpath - path;
    
		strncpy(path_string, path, len);
		path_string[len] = '\0';
    
		PyObject *newpath = PyList_New(5);
		PyList_SetItem(newpath, 0, PyString_FromString(path_string));
    
		g_pSM->BuildPath(SourceMod::Path_SM, path_string, PLATFORM_MAX_PATH, "extensions/viper/lib");
    
		/* Let's keep the folder separators consistent */
		StrReplace(path_string, "\\", "/", PLATFORM_MAX_PATH);
		PyList_SetItem(newpath, 1, PyString_FromString(path_string));
    
		PyList_SetItem(newpath, 2, PyString_FromFormat("%s/lib-dynload", path_string));
		PyList_SetItem(newpath, 3, PyString_FromFormat("%s/site-packages", path_string));
    
	#ifdef WIN32
		g_pSM->BuildPath(SourceMod::Path_SM, path_string, PLATFORM_MAX_PATH,
			"extensions/viper/lib/plat-win");
	#else
		g_pSM->BuildPath(SourceMod::Path_SM, path_string, PLATFORM_MAX_PATH,
			"extensions/viper/lib/plat-linux2");
	#endif
		StrReplace(path_string, "\\", "/", PLATFORM_MAX_PATH);
		PyList_SetItem(newpath, 4, PyString_FromString(path_string));
    
		delete [] path_string;
    
		// SetObject increases the refcnt by one
		PySys_SetObject("path", newpath);
		Py_DECREF(newpath);
    
		/* Set the custom excepthook, so that the annoying "Error in sys.excepthook:"
		 * doesn't happen. The cause of it is that when an exception occurs, too
		 * much of the sys module is unloaded to handle the exception correctly.
		 */
		PyObject *tb = PyImport_ImportModule("traceback");
		PyObject *print_exception = PyObject_GetAttrString(tb, "print_exception");
		PySys_SetObject("excepthook", print_exception);
    
		Py_DECREF(tb);
		Py_DECREF(print_exception);
    
		/* Create the plug-in's globals dict */
		PyObject *__main__ = PyImport_ImportModule("__main__");
		PyObject *pluginDict = PyModule_GetDict(__main__);
		Py_DECREF(__main__);
    
		/* Add __file__ to the plug-in */
		PyDict_SetItemString(pluginDict, "__file__", PyString_FromString(path));
    
		/* Initialize the sourcemod Python module. */
		initsourcemod();
    
		return pluginDict;
	}

	void
	CPlugin::RunPlugin()
	{
		PyThreadState_Swap(m_pThreadState);

		m_pPluginDict = InitializePlugin(m_sPath);
    
		/* Run the plug-in file.
		 * We use Python's File object so that the underlying C FILE object will always match
		 * the one used when Python was compiled.
		 */
		PyObject *pyfile = PyFile_FromString(m_sPath, "r");
		PyRun_File(PyFile_AsFile(pyfile), m_sPath, Py_file_input, m_pPluginDict, m_pPluginDict);
		Py_DECREF(pyfile);
    
		/* Save the error if one has occurred, and print it out to the server */
		if (PyErr_Occurred())
		{
			PyObject *errobj, *errdata, *errtraceback;
			PyObject *pystring = NULL;
        
			/* This clears the current error; we must restore it manually later */
			PyErr_Fetch(&errobj, &errdata, &errtraceback);
        
			pystring = PyObject_Str(errobj);
			if (errobj != NULL || pystring != NULL)
				m_psErrorType = sm_strdup(PyString_AsString(pystring));
			else
				m_psErrorType = sm_strdup("<no exception type>");
        
			Py_XDECREF(pystring);
        
			pystring = PyObject_Str(errdata);
			if (errdata != NULL || pystring != NULL)
				m_psError = sm_strdup(PyString_AsString(pystring));
			else
				m_psError = sm_strdup("<no exception data>");
        
			Py_XDECREF(pystring);
        
			if (errtraceback != NULL)
			{
				Py_INCREF(errtraceback);
				m_pErrorTraceback = errtraceback;
			}
        
			PyErr_Restore(errobj, errdata, errtraceback);
			PyErr_Print();
        
			Py_XDECREF(errobj);
			Py_XDECREF(errdata);
			/* We don't decref errtraceback because it's saved to the plug-in */
        
			m_status = ViperPlugin_Error;
		}
		else
		{
			m_status = ViperPlugin_Running;
			UpdateInfo();
		}
	}

	void
	CPlugin::UpdateInfo()
	{
	#define RETRIEVE_INFO_FIELD(field)\
		{ \
			PyObject *_obj; \
			if ((_obj = PyDict_GetItemString(myinfo, #field)) != NULL) \
			{ \
				PyObject *str = PyObject_Str(_obj); \
				m_info.field = sm_strdup(PyString_AsString(str)); \
				Py_DECREF(str); \
			} \
		}
    
		PyObject *myinfo = PyDict_GetItemString((PyObject*)m_pPluginDict, "myinfo");
		if (myinfo == NULL || !PyDict_Check(myinfo))
			return;
    
		RETRIEVE_INFO_FIELD(name);
		RETRIEVE_INFO_FIELD(author);
		RETRIEVE_INFO_FIELD(description);
		RETRIEVE_INFO_FIELD(version);
		RETRIEVE_INFO_FIELD(url);
	}

	bool
	CPlugin::SetProperty(char const *prop, void *value)
	{
		return sm_trie_insert(m_pProps, prop, value);
	}

	bool
	CPlugin::GetProperty(char const *prop, void **ptr, bool remove)
	{
		void *temp;

		if (!sm_trie_retrieve(m_pProps, prop, (void **)&temp))
			return false;
    
		*ptr = temp;

		if (remove)
		{
			sm_trie_delete(m_pProps, prop);
			return true;
		}

		return true;
	}

	/*********************
	 * PLUGIN MANAGER
	 *********************/
	CPluginManager::CPluginManager()
	{
		m_trie = sm_trie_create();
		m_LoadingLocked = false;
	}

	CPluginManager::~CPluginManager()
	{
		sm_trie_destroy(m_trie);
	}

	void
	CPluginManager::OnViperShutdown()
	{
		SourceHook::List<CPlugin *>::iterator iter;
		for (iter=m_list.begin(); !m_list.empty() && iter!=m_list.end(); iter++)
		{
			UnloadPlugin((*iter));
		}
	}

	bool
	CPluginManager::AddPluginsListener(IViperPluginsListener *listener)
	{
		if (listener == NULL)
			return false;
    
		m_Listeners.push_back(listener);
		return true;
	}

	void
	CPluginManager::LoadPluginsFromDir(char const *dir)
	{
		char error[256];
		SourceMod::IDirectory *pDir = libsys->OpenDirectory(dir);
		if (pDir == NULL)
		{
			libsys->GetPlatformError(error, sizeof(error));

			g_pSM->LogError(myself, "Could not load plugins from %s", dir);
			g_pSM->LogError(myself, "Platform returned error: %s", error);

			return;
		}

		char pyinit[PLATFORM_MAX_PATH];

		while (pDir->MoreFiles())
		{
			if (pDir->IsEntryDirectory()
				&& strcmp(pDir->GetEntryName(), ".") != 0
				&& strcmp(pDir->GetEntryName(), "..") != 0
				&& strcmp(pDir->GetEntryName(), "disabled") != 0
				&& strcmp(pDir->GetEntryName(), "optional") != 0)
			{
				UTIL_Format(pyinit, sizeof(pyinit), "%s/%s/__init__.py", dir,
					pDir->GetEntryName());

				if (libsys->PathExists(pyinit))
				{
					g_VPlugins.LoadPlugin((char const *)pyinit,
						ViperPluginType_MapOnly, error, sizeof(error), NULL);
				}
			}

			pDir->NextEntry();
		}

		libsys->CloseDirectory(pDir);
	}

	CPlugin *
	CPluginManager::LoadPlugin(char const *path, ViperPluginType type,
		char error[], size_t maxlength, bool *wasloaded)
	{
		CPlugin *pl;
		ViperLoadRes res;

		if ((res = _LoadPlugin(&pl, path, type, error, maxlength)) == ViperLoadRes_Failure)
		{
			if(pl == NULL)
				return NULL;
			else
				return pl;
		}
    
		if (res == ViperLoadRes_AlreadyLoaded)
		{
			*wasloaded = true;
			return pl;
		} 
		else if (res == ViperLoadRes_NeverLoad)
		{
			if (m_LoadingLocked)
				UTIL_Format(error, maxlength, "There is a global plugin loading "
					"lock in effect");
			else
				UTIL_Format(error, maxlength, "This plugin is blocked from loading "
					"(see ViperPlugin_settings.cfg)");
			return NULL;
		}
    
		SourceHook::List<IViperPluginsListener *>::iterator iter;
		for (iter=m_Listeners.begin(); iter!=m_Listeners.end(); iter++)
		{
			(*iter)->OnPluginLoaded(pl);
		}

		return pl;
	}

	ViperLoadRes
	CPluginManager::_LoadPlugin(CPlugin **_plugin, char const *path,
		ViperPluginType type, char error[], size_t maxlength)
	{
		if (m_LoadingLocked)
			return ViperLoadRes_NeverLoad;

		CPlugin *pPlugin;
    
		size_t pathsize = strlen(path);
		char *path_slashes = new char[pathsize];
		strcpy(path_slashes, path);
		StrReplace(path_slashes, "\\", "/", pathsize);
    
		if (sm_trie_retrieve(m_trie, path_slashes, (void **)&pPlugin))
		{
			if (pPlugin->GetStatus() == ViperPlugin_BadLoad)
				UnloadPlugin(pPlugin);
			else
			{
				*_plugin = pPlugin;
				return ViperLoadRes_AlreadyLoaded;
			}
		}
    
		pPlugin = CPlugin::CreatePlugin(path_slashes, error, maxlength);
		assert(pPlugin != NULL);

		pPlugin->m_type = ViperPluginType_MapUpdated;

		sm_trie_insert(m_trie, path_slashes, pPlugin);
		m_list.push_back(pPlugin);

		/* Setup Python aspects of plug-in and begin execution */
		pPlugin->RunPlugin();
    
		*_plugin = pPlugin;
    
		if (pPlugin->GetStatus() != ViperPlugin_Running)
		{
			strncpy(error, (pPlugin->GetError() != NULL ? pPlugin->GetError() : ""),
					maxlength);
			return ViperLoadRes_Failure;
		}

		return ViperLoadRes_Successful;
	}

	bool CPluginManager::ReloadPlugin(CPlugin *plugin)
	{
		char filename[PLATFORM_MAX_PATH];
		bool wasloaded;
		ViperPluginType ptype;

		strcpy(filename, plugin->GetPath());
		ptype = plugin->GetType();

		if (!UnloadPlugin(plugin))
			return false;

		CPlugin *newpl;
		if ((newpl = LoadPlugin(filename, ptype, NULL, 0, &wasloaded)) == NULL)
			return false;
    
		if (newpl->GetStatus() == ViperPlugin_Error)
			return false;

		return true;
	}

	bool
	CPluginManager::UnloadPlugin(CPlugin *plugin)
	{
		// If an error has occurred, the thread state may contain a bad exception,
		// causing the PyDict_GetItemString call to crash.
		if (PyErr_Occurred())
			PyErr_Clear();
    
		if (plugin->GetStatus() == ViperPlugin_Running)
		{
			// Before calling listeners, tell the plug-in we're unloading
			PyObject *unload = PyDict_GetItemString(plugin->m_pPluginDict, "plugin_unload");
			if (unload != NULL && PyCallable_Check(unload))
			{
				PyObject_CallObject(unload, NULL);
				if (PyErr_Occurred())
					PyErr_Print();
			}
		}
    
		SourceHook::List<IViperPluginsListener *>::iterator iter;
		for (iter=m_Listeners.begin(); iter!=m_Listeners.end(); iter++)
		{
			(*iter)->OnPluginUnloaded(plugin);
		}
    
		sm_trie_delete(m_trie, plugin->GetPath());
		m_list.remove(plugin);
    
		PyThreadState *_save = PyThreadState_Get();
		delete plugin;
		PyThreadState_Swap(_save);

		return true;
	}

	IViperPlugin *
	CPluginManager::GetPluginOfInterpreterState(PyInterpreterState *interp)
	{
		IViperPlugin *interp_plugin = g_VConsole.GetInterpPlugin();
		if (interp_plugin != NULL && interp_plugin->GetInterpState() == interp)
			return interp_plugin;
    
		SourceHook::List<CPlugin *>::iterator iter;
		for (iter=m_list.begin(); iter!=m_list.end(); iter++)
		{
			if ((*iter)->GetInterpState() == interp)
				return (*iter);
		}
    
		return NULL;
	}

	CPlugin *
	CPluginManager::GetPluginByPath(char const *path)
	{
		CPlugin *pPlugin;
    
		char path_slashes[PLATFORM_MAX_PATH];
		strncopy((char*)&path_slashes, path, sizeof(path_slashes));
		StrReplace(path_slashes, "\\", "/", sizeof(path_slashes));

		if (!sm_trie_retrieve(m_trie, path_slashes, (void **)&pPlugin))
		{
			if (!sm_trie_retrieve(m_trie, GetLastOfPath(path), (void **)&pPlugin))
				return NULL;
		}

		return pPlugin;
	}

	CPlugin *
	CPluginManager::GetPluginByOrder(int num)
	{
		if (num < 1 || num > GetPluginCount())
			return NULL;
    
		int id = 1;
		SourceHook::List<CPlugin *>::iterator iter = m_list.begin();
    
		for (; iter != m_list.end() && id < num; id++, iter++ ){}

		return (*iter);
	}

	CPlugin *
	CPluginManager::FindPluginByConsoleArg(char const *arg)
	{
		int id;
		char *end = NULL;
		CPlugin *pl;
    
		/* By order first */
		id = strtol(arg, &end, 10);
		if (end != NULL && *end == '\0')
		{
			pl = GetPluginByOrder(id);
			if (pl != NULL)
				return pl;
		}
    
		/* Then path */
		pl = GetPluginByPath(arg);
		if (pl != NULL)
			return pl;
    
		/* XXX: Deepest folder name, should be path relative to plugins/ */
		char const *folder = GetLastOfPath(arg);
		SourceHook::List<CPlugin *>::iterator iter;
		for(iter = m_list.begin(); iter != m_list.end(); iter++)
		{
			if(stricmp(folder, (*iter)->GetFolder()) == 0)
				return (*iter);
		}
    
		/* Finally by plug-in name */
		for(iter = m_list.begin(); iter != m_list.end(); iter++)
		{
			if(stricmp(arg, (*iter)->GetName()) == 0)
				return (*iter);
		}

		return NULL;
	}

	int
	CPluginManager::GetPluginCount()
	{
		return m_list.size();
	}

	void
	CPluginManager::OnRootConsoleCommand(char const *cmdname,
		const CCommand &command)
	{
	#   define CheckLoadLock() if (m_LoadingLocked) \
		{\
			g_pMenu->ConsolePrint("[Viper] There is a loading lock in effect. No plug-ins can be loaded."); \
			return; \
		}

		char const *cmd = command.Arg(3);
		int argc = command.ArgC();

		if (strcmp(cmd, "list") == 0)
		{
			int plnum = GetPluginCount();
        
			if (!plnum)
			{
				g_pMenu->ConsolePrint("[Viper] No plugins loaded");
				return;
			}

			g_pMenu->ConsolePrint("[Viper] Listing %d plugin%s", plnum,
				plnum > 1 ? "s" : "");

			SourceHook::List<CPlugin *>::iterator iter;
			const viper_plugininfo_t *info;
        
			int i = 0;
			for (iter=m_list.begin(); iter != m_list.end(); iter++)
			{
				info = (*iter)->GetPublicInfo();
				g_SMAPI->ConPrintf("  %02d \"%s\"", ++i, info->name);

				if (IS_STR_FILLED(info->version))
					g_SMAPI->ConPrintf(" (%s)", info->version);

				if (IS_STR_FILLED(info->author))
					g_SMAPI->ConPrintf(" by %s", info->author);
            
				g_SMAPI->ConPrint("\n");
			}

			return;
		}
		else if (strcmp(cmd, "load") == 0)
		{
			if (argc < 5)
			{
				g_pMenu->ConsolePrint("[Viper] Usage: sm py plugins load <name|path>");
				return;
			}
        
			CheckLoadLock();

			char const *plstr = command.Arg(4);
        
			char pl[PLATFORM_MAX_PATH];
			g_pSM->BuildPath(SourceMod::Path_SM, pl, sizeof(pl), "plugins/%s/__init__.py", plstr);
			StrReplace(pl, "\\", "/", sizeof(pl));

			if (!libsys->PathExists(pl))
			{
				g_pMenu->ConsolePrint("[Viper] Could not find plugin \"%s\"", plstr);
				return;
			}
        
			char error[256];
			bool wasloaded = false;
			CPlugin *pPlugin;

			if ((pPlugin = LoadPlugin(pl, ViperPluginType_MapOnly, error,
				sizeof(error), &wasloaded)) == NULL)
			{
				g_pMenu->ConsolePrint("[Viper] Could not load plugin \"%s\": %s",
					plstr, error);
				return;
			}
        
			if (wasloaded)
			{
				g_pMenu->ConsolePrint("[Viper] Plugin \"%s\" is already loaded",
					pPlugin->GetName());
				return;
			}
        
			if (pPlugin->GetStatus() == ViperPlugin_Error)
			{
				g_pMenu->ConsolePrint("[Viper] Plugin \"%s\" loaded erroneously.",
					pPlugin->GetName());
				return;
			}
        
			g_pMenu->ConsolePrint("[Viper] Successfully loaded plugin \"%s\"!",
				pPlugin->GetName());
        
			return;
		}
		else if (strcmp(cmd, "unload") == 0)
		{
			if (argc < 5)
			{
				g_pMenu->ConsolePrint("[Viper] Usage: sm py plugins unload <#|name>");
				return;
			}
        
			char const *plstr = command.Arg(4);
			CPlugin *pl = FindPluginByConsoleArg(plstr);
        
			if (pl == NULL)
			{
				g_pMenu->ConsolePrint("[Viper] Could not find plugin \"%s\"", plstr);
				return;
			}
        
			/* Copy over the plugin's name before we delete the plugin */
			char const *name = sm_strdup(pl->GetName());
        
			UnloadPlugin(pl);
			g_pMenu->ConsolePrint("[Viper] Plugin \"%s\" unloaded!", name);
        
			delete [] name;
        
			return;
		}
		else if (strcmp(cmd, "load_lock") == 0)
		{
			if (m_LoadingLocked)
				g_pMenu->ConsolePrint("[Viper] There is already a loading lock in "
					"effect.");
			else
			{
				m_LoadingLocked = true;
				g_pMenu->ConsolePrint("[Viper] Loading is now locked; no plugins "
					"will be loaded or re-loaded.");
			}
			return;
		}
		else if (strcmp(cmd, "load_unlock") == 0)
		{
			if (m_LoadingLocked)
			{
				m_LoadingLocked = false;
				g_pMenu->ConsolePrint("[Viper] The loading lock is no longer in effect.");
			}
			else
				g_pMenu->ConsolePrint("[Viper] There was no loading lock in effect.");
        
			return;
		}
		else if (strcmp(cmd, "info") == 0)
		{
			if (argc < 5)
			{
				g_pMenu->ConsolePrint("[Viper] Usage: sm py plugins info <#|name>");
				return;
			}

			char const *arg = command.Arg(4);
			CPlugin *pl = FindPluginByConsoleArg(arg);

			if (pl == NULL)
			{
				g_pMenu->ConsolePrint("[Viper] Could not find plugin \"%s\"", arg);
				return;
			}

			const viper_plugininfo_t *info = pl->GetPublicInfo();
    
			if (pl->GetStatus() <= ViperPlugin_Error)
			{
				if (IS_STR_FILLED(info->name))
				{
					if (IS_STR_FILLED(info->description))
						g_pMenu->ConsolePrint("  Title: %s (%s)", info->name,
							info->description);
					else
						g_pMenu->ConsolePrint("  Title: %s", info->name);
				}
            
				if (IS_STR_FILLED(info->author))
					g_pMenu->ConsolePrint("  Author: %s", info->author);

				if (IS_STR_FILLED(info->version))
					g_pMenu->ConsolePrint("  Version: %s", info->version);

				if (IS_STR_FILLED(info->url))
					g_pMenu->ConsolePrint("  URL: %s", info->url);

				if (pl->GetStatus() == ViperPlugin_Error)
				{
					g_pMenu->ConsolePrint("  Error: %s %s", pl->m_psErrorType,
						pl->m_psError);
                
					PyThreadState *_save = PyThreadState_Get();
					PyThreadState_Swap(pl->GetThreadState());
                
					PyObject *tb_strings = NULL;
					PyObject *format_tb = NULL;
					PyObject *traceback = NULL;
                
					traceback = PyImport_ImportModule("traceback");
					if (!traceback)
						goto done;

					format_tb = PyObject_GetAttrString(traceback, "format_tb");
					if (!format_tb || !PyCallable_Check(format_tb))
						goto done;
                
					tb_strings = PyObject_CallObject(format_tb, Py_BuildValue("(O)",
						pl->m_pErrorTraceback));
					if (!tb_strings)
						goto done;
                
					Py_ssize_t i;
					Py_ssize_t size;
                
					size = PyList_Size(tb_strings);
					/* Start at frame #1, because frame #0 will always be the
					 * execfile() frame: File "<string>", line 1, in <module>
					 */
					i = (size > 1) ? 1 : 0;
                
					for (; i < size; i++)
					{
						g_pMenu->ConsolePrint("  %s", PyString_AsString(
							PyList_GetItem(tb_strings, i)));
					}
                
				done:
					Py_XDECREF(traceback);
					Py_XDECREF(format_tb);
					Py_XDECREF(tb_strings);
                
					PyThreadState_Swap(_save);
					return;
				}
				else
				{
					if (pl->GetStatus() == ViperPlugin_Running)
						g_pMenu->ConsolePrint("  Status: running");
					else
						g_pMenu->ConsolePrint("  Status: not running");

					char const *typestr = "";
					switch (pl->GetType())
					{
						case ViperPluginType_MapUpdated:
							typestr = "Map Change if Updated";
							break;
						case ViperPluginType_MapOnly:
							typestr = "Map Change";
							break;
						case ViperPluginType_Private:
						case ViperPluginType_Global:
							typestr = "Never";
							break;
					}

					g_pMenu->ConsolePrint("  Reloads: %s", typestr);
				}
			}
			else
			{
				g_pMenu->ConsolePrint("  File info: (title \"%s\") (version \"%s\")", 
					info->name, info->version ? info->version : "<none>");
            
				if (IS_STR_FILLED(info->url))
					g_pMenu->ConsolePrint("  File URL: %s", info->url);
			}

			return;
		}
		else if (strcmp(cmd, "reload") == 0)
		{
			if (argc < 5)
			{
				g_pMenu->ConsolePrint("[Viper] Usage: sm py plugins info <#|name>");
				return;
			}
        
			CheckLoadLock();

			char const *plstr = command.Arg(4);
			CPlugin *pl = FindPluginByConsoleArg(plstr);

			if (pl == NULL)
			{
				g_pMenu->ConsolePrint("[Viper] Could not find plugin \"%s\"",
					plstr);
				return;
			}

			char *name = sm_strdup(pl->GetName());

			if (ReloadPlugin(pl))
				g_pMenu->ConsolePrint("[Viper] Plugin '%s' successfully reloaded!", name);
			else
				g_pMenu->ConsolePrint("[Viper] Failed to reload plugin \"%s\"", name);

			delete [] name;

			return;
		}
		else if (strcmp(cmd, "refresh") == 0)
		{
			// TODO: Make this code less disgusting.
        
			CheckLoadLock();
        
			char plugins_path[PLATFORM_MAX_PATH];
			g_pSM->BuildPath(SourceMod::Path_SM, plugins_path, sizeof(plugins_path),
				"plugins");
			SourceMod::IDirectory *pDir = libsys->OpenDirectory(plugins_path);
        
			char error[256];
			if (pDir == NULL)
			{
				libsys->GetPlatformError(error, sizeof(error));

				g_pSM->LogError(myself, "Could not open \"%s\"", plugins_path);
				g_pSM->LogError(myself, "Platform returned error: %s", error);

				return;
			}

			char pyinit[PLATFORM_MAX_PATH];
			size_t loaded = 0;
			while (pDir->MoreFiles())
			{
				if (pDir->IsEntryDirectory()
					&& strcmp(pDir->GetEntryName(), ".") != 0
					&& strcmp(pDir->GetEntryName(), "..") != 0
					&& strcmp(pDir->GetEntryName(), "disabled") != 0
					&& strcmp(pDir->GetEntryName(), "optional") != 0)
				{
					UTIL_Format(pyinit, sizeof(pyinit), "%s/%s/__init__.py", plugins_path,
						pDir->GetEntryName());

					if (libsys->PathExists(pyinit) &&
						!sm_trie_retrieve(m_trie, pyinit, NULL))
					{
						META_CONPRINTF("%s\n", pyinit);
						g_VPlugins.LoadPlugin((char const *)pyinit,
							ViperPluginType_MapOnly, error, sizeof(error), NULL);
						loaded++;
					}
				}

				pDir->NextEntry();
			}

			libsys->CloseDirectory(pDir);
        
			if (loaded)
				g_pMenu->ConsolePrint("[Viper] Loaded %d plug-ins", loaded);
			else
				g_pMenu->ConsolePrint("[Viper] No new plug-ins to load!");
        
			return;
		}
		else if (strcmp(cmd, "unload_all") == 0)
		{
				SourceHook::List<CPlugin *>::iterator iter;
				for (iter=m_list.begin(); iter!=m_list.end(); iter++)
				{
					UnloadPlugin((*iter));
				}
        
			g_pMenu->ConsolePrint("[Viper] Unloaded all plugins!");
        
			return;
		}
    
		g_pMenu->ConsolePrint("Viper Plugins Menu:");
		g_pMenu->DrawGenericOption("info", "Information about a plugin");
		g_pMenu->DrawGenericOption("list", "Show loaded plugins");
		g_pMenu->DrawGenericOption("load", "Load a plugin");
		g_pMenu->DrawGenericOption("load_lock", "Prevents any more plugins from being loaded");
		g_pMenu->DrawGenericOption("load_unlock", "Re-enables plugin loading");
		g_pMenu->DrawGenericOption("refresh", "Loads any plugins not already loaded in the plugin folder");
		g_pMenu->DrawGenericOption("reload", "Reloads a plugin");
		g_pMenu->DrawGenericOption("unload", "Unloads a plugin");
		g_pMenu->DrawGenericOption("unload_all", "Unloads all plugins");
		return;
	}

	CPluginManager g_VPlugins;
}


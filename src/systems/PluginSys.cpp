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

#include <Python.h>
#include "PluginSys.h"
#include "ConCmdManager.h"

PyObject *
CPluginFunction::Execute(PyObject *args, PyObject *keywds)
{
    PyThreadState *_save = PyThreadState_Get();
    PyThreadState_Swap(m_pPlugin->GetThreadState());
    
    if (!PyCallable_Check(m_pFunc))
        return NULL;
    
    PyObject *result = PyObject_Call(m_pFunc, args, keywds);
    
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

CPluginFunction *
CPluginFunction::CreatePluginFunction(PyFunction *func, IViperPlugin *pl)
{
    CPluginFunction *plfunc = NULL;
    
    PyThreadState *_save = PyThreadState_Get();
    PyThreadState_Swap(pl->GetThreadState());
    
    if (!PyCallable_Check(func))
        goto end;
    
    plfunc = new CPluginFunction;
    plfunc->m_pFunc = func;
    plfunc->m_pPlugin = pl;
    
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

    snprintf(m_sPath, sizeof(m_sPath), "%s", file);
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
        Py_XDECREF(m_pErrorTraceback);
        m_pErrorTraceback = NULL;
    }
    
    /* Destroy the plug-in's sub-interpreter and switch back to main */
    if (m_pThreadState != NULL)
    {
        PyThreadState_Swap(m_pThreadState);
        
        if (GetStatus() == ViperPlugin_Running)
            Py_EndInterpreter(m_pThreadState);
        else
        {
            PyThreadState_Swap(NULL);
            PyThreadState_Clear(m_pThreadState);
            PyThreadState_Delete(m_pThreadState);
        }
    }
    
}

CPlugin *
CPlugin::CreatePlugin(char const *path, char* error, size_t maxlength)
{
    CPlugin *pPlugin = new CPlugin(path);
    assert(pPlugin != NULL);

    if (!libsys->PathExists(path))
    {
        pPlugin->m_status = ViperPlugin_BadLoad;
        return pPlugin;
    }

    return pPlugin;
}

void
CPlugin::RunPlugin()
{
    m_sFolder = ParseNiceName(m_sPath);
    
    m_info.name = m_sFolder;
    m_info.author = "";
    m_info.description = "";
    m_info.version = "";
    m_info.url = "";
    
    m_pThreadState = Py_NewInterpreter();
    if (m_pThreadState == NULL)
    {
        g_pSM->LogError(myself, "Fatal error in creating Python "
            "sub-interpreter for %s!", m_sFolder);
        g_VPlugins.UnloadPlugin(this);
        return;
    }
    
    PyObject *thread_dict = PyThreadState_GetDict();
    PyDict_SetItemString(thread_dict, "viper_cplugin", PyCObject_FromVoidPtr(
        (void*)this, NULL));
    
    PyThreadState_Swap(m_pThreadState);
    
    /* Clear sys.path and add the plug-in's folder */
    char *plfolder = (char*)malloc(strlen(m_sPath));
    unsigned int len = strrchr(m_sPath, '/') - m_sPath;
    strncpy(plfolder, m_sPath, len);
    plfolder[len] = '\0';
    
    PyObject *newpath = PyList_New(1);
    PyList_SetItem(newpath, 0, PyString_FromString(plfolder));
    delete [] plfolder;
    
    PyObject *sys = PyImport_AddModule("sys");
    PyObject_SetAttrString(sys, "path", newpath);
    
    /* Create the plug-in's globals dict */
    m_pPluginDict = PyModule_GetDict(PyImport_ImportModule("__main__"));
    
    FILE *fp = fopen(m_sPath, "r");
    PyRun_File(fp, m_sPath, Py_file_input, m_pPluginDict, m_pPluginDict);
    fclose(fp);
    
    /* Save the critical error, then print it out to the user */
    if (PyErr_Occurred())
    {
        PyObject *errobj,
                 *errdata,
                 *errtraceback,
                 *pystring = NULL;
        
        /* This clears the current error; we must restore it manually later */
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        
        if (errobj != NULL && PyString_Check(pystring = PyObject_Str(errobj)))
            m_psErrorType = sm_strdup(PyString_AsString(pystring));
        else
            m_psErrorType = sm_strdup("<no exception type>");
        
        Py_XDECREF(pystring);
        
        if (errdata != NULL && PyString_Check(pystring = PyObject_Str(errdata)))
            m_psErrorType = sm_strdup(PyString_AsString(pystring));
        else
            m_psErrorType = sm_strdup("<no exception data>");
        
        Py_XDECREF(pystring);
        
        if (errtraceback != NULL)
            m_pErrorTraceback = errtraceback;
        
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
    
    PyThreadState_Swap(NULL);
}

void
CPlugin::UpdateInfo()
{
#define RETRIEVE_INFO_FIELD(field) {\
    PyObject *_obj;\
    if ((_obj = PyDict_GetItemString(myinfo, #field)) != NULL)\
        m_info.field = sm_strdup(PyString_AsString(PyObject_Str(_obj)));}
    
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
    
    SourceHook::List<CPlugin *>::iterator iter;
    for (iter=m_list.begin(); iter!=m_list.end(); iter++)
    {
        SourceHook::List<IViperPluginListener *>::iterator listen;
        for (listen=m_Listeners.begin(); listen!=m_Listeners.end(); listen++)
        {
            (*listen)->OnPluginCreated((*iter));
        }
    }
}

bool
CPluginManager::AddPluginListener(IViperPluginListener *listener)
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
                g_VPlugins.LoadPlugin((char const *)pyinit, false,
                    ViperPluginType_MapOnly, error, sizeof(error), NULL);
            }
        }

        pDir->NextEntry();
    }

    libsys->CloseDirectory(pDir);
}

CPlugin *
CPluginManager::LoadPlugin(char const *path, bool debug, ViperPluginType type,
    char error[], size_t maxlength, bool *wasloaded)
{
	CPlugin *pl;
	ViperLoadRes res;

	if ((res = _LoadPlugin(&pl, path, debug, type, error, maxlength)) == ViperLoadRes_Failure)
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

	if (res == ViperLoadRes_NeverLoad)
	{
		if (m_LoadingLocked)
			UTIL_Format(error, maxlength, "There is a global plugin loading "
			    "lock in effect");
		else
			UTIL_Format(error, maxlength, "This plugin is blocked from loading "
			    "(see ViperPlugin_settings.cfg)");
		return NULL;
	}
	
    SourceHook::List<IViperPluginListener *>::iterator iter;
    for (iter=m_Listeners.begin(); iter!=m_Listeners.end(); iter++)
    {
        (*iter)->OnPluginLoaded(pl);
    }

	return pl;
}

ViperLoadRes
CPluginManager::_LoadPlugin(CPlugin **_plugin, char const *path, bool debug,
    ViperPluginType type, char error[], size_t maxlength)
{
    if (m_LoadingLocked)
        return ViperLoadRes_NeverLoad;

    CPlugin *pPlugin;
    char const *folder = ParseNiceName(path);

    if (sm_trie_retrieve(m_trie, folder, (void **)&pPlugin))
    {
        if (pPlugin->GetStatus() == ViperPlugin_BadLoad)
            UnloadPlugin(pPlugin);
        else
        {
            *_plugin = pPlugin;
            
            return ViperLoadRes_AlreadyLoaded;
        }
    }
    

    pPlugin = CPlugin::CreatePlugin(path, error, maxlength);

    assert(pPlugin != NULL);

    pPlugin->m_type = ViperPluginType_MapUpdated;

    sm_trie_insert(m_trie, folder, pPlugin);
    m_list.push_back(pPlugin);

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
	if ((newpl = LoadPlugin(filename, false, ptype, NULL, 0, &wasloaded)) == NULL)
		return false;
	
	if (newpl->GetStatus() == ViperPlugin_Error)
	    return false;

	return true;
}

bool
CPluginManager::UnloadPlugin(CPlugin *plugin)
{
    SourceHook::List<IViperPluginListener *>::iterator iter;
    for (iter=m_Listeners.begin(); iter!=m_Listeners.end(); iter++)
    {
        (*iter)->OnPluginUnloaded(plugin);
    }
	
	sm_trie_delete(m_trie, plugin->GetFolder());
	m_list.remove(plugin);

	delete plugin;

	return true;
}

CPlugin *
CPluginManager::GetPluginByPath(char const *path)
{
	CPlugin *pPlugin;

	if (!sm_trie_retrieve(m_trie, ParseNiceName(path), (void **)&pPlugin))
		return NULL;

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
	char *end;
	CPlugin *pl;
	
	/* By order first */
	id = strtol(arg, &end, 10);
	if (*end == '\0')
	{
		pl = GetPluginByOrder(id);
		if (pl != NULL)
			return pl;
	}
    
    /* Then folder name */
    if (sm_trie_retrieve(m_trie, arg, (void **)&pl))
        return pl;
    
    /* Finally by plug-in name */
	SourceHook::List<CPlugin *>::iterator iter;
	for(iter = m_list.begin(); iter != m_list.end(); iter++)
	{
		if(strcmp(arg, (*iter)->GetName()) == 0)
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
    /* :TODO: 
    - plugins
      - refresh */

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
        bool wasloaded;
        CPlugin *pPlugin;

        if ((pPlugin = LoadPlugin(pl, false, ViperPluginType_MapOnly, error,
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
                
                PyThreadState_Swap(pl->GetThreadState());
                
                PyObject *tb_strings = NULL;
                PyObject *format_tb = NULL;
                PyObject *traceback = NULL;
                int size;
                
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
                
                int i;
                size = PyList_Size(tb_strings);
                /* Start at frame #1, because frame #0 will always be the
                 * execfile() frame: File "<string>", line 1, in <module>
                 */
                for (i = 1; i < size; i++)
                {
                    g_pMenu->ConsolePrint("  %s", PyString_AsString(
                        PyList_GetItem(tb_strings, i)));
                }
                
            done:
                Py_XDECREF(traceback);
                Py_XDECREF(format_tb);
                Py_XDECREF(tb_strings);
                
                PyThreadState_Swap(NULL);
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
            /* TODO */
            /*g_pMenu->ConsolePrint("  Load error: %s", pl->m_errormsg);*/
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
    
    g_pMenu->ConsolePrint("Viper Plugins Menu:");
    g_pMenu->DrawGenericOption("info", "Information about a plugin");
    g_pMenu->DrawGenericOption("list", "Show loaded plugins");
    g_pMenu->DrawGenericOption("load", "Load a plugin");
    g_pMenu->DrawGenericOption("load_lock", "Prevents any more plugins from being loaded");
    g_pMenu->DrawGenericOption("load_unlock", "Re-enables plugin loading");
    //g_pMenu->DrawGenericOption("refresh", "Reloads/refreshes all plugins in the plugin folder");
    g_pMenu->DrawGenericOption("reload", "Reloads a plugin");
    g_pMenu->DrawGenericOption("unload", "Unloads a plugin");
    return;
}

CPluginManager g_VPlugins;


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

#ifndef _INCLUDE_VIPER_PLUGINSYS_H_
#define _INCLUDE_VIPER_PLUGINSYS_H_

#include <Python.h>
#include "viper_globals.h"
#include "sm_trie.h"
#include <sh_list.h>
#include <convar.h>
#include <compat_wrappers.h>
#include <IViperPluginSys.h>

class CPluginFunction : public IViperPluginFunction
{
public: //IViperPluginFunction
    virtual PyObject *Execute(PyObject *args, PyObject *keywds=NULL);
    virtual IViperPlugin *GetOwnerPlugin();
    virtual PyObject *GetFunction();
public:
    static CPluginFunction *CreatePluginFunction(PyObject *func,
                                                 IViperPlugin *pl);

protected:
    IViperPlugin *m_pPlugin;
    PyObject *m_pFunc;
};

/**
 * @brief Handles the actual loading and execution of plug-ins
 */
class CPlugin : public IViperPlugin
{
    friend class CPluginManager;
    
public:
    CPlugin(char const * file);
    ~CPlugin();
    
    /**
     * Creates a new CPlugin and begins the loading process
     *
     * @param path  The path of the plug-in (eg, "C:/__init__.py")
     * @param error An optional buffer for errors
     * @param maxlength The size of the buffer
     */
    static CPlugin *CreatePlugin(char const *path, char* error, size_t maxlength);
    
    /**
     * Runs the plug-in
     *
     * @note This must be called to execute the plug-in.
     */
    virtual void RunPlugin();
    
    /**
     * Retrieves the plug-in's "myinfo" global variable to store inside this
     * CPlugin, for use in the main menu.
     * @note: This does NOT release/acquire the interpreter lock, and must be
     *   done manually by the caller.
     */
    virtual void UpdateInfo();
    
    /// Returns the path to the script
    virtual char const *GetPath()
    {
        return m_sPath;
    }
    
    virtual PyThreadState *GetThreadState()
    {
        return m_pThreadState;
    }
    
    virtual char const *GetFolder()
    {
        return m_sFolder;
    }
    
    virtual ViperPluginStatus GetStatus()
    {
        return m_status;
    }
    
    virtual ViperPluginType GetType()
    {
        return m_type;
    }
    
    virtual char const *GetName()
    {
        return m_info.name;
    }
    
    virtual const viper_plugininfo_t *GetPublicInfo()
    {
        return &m_info;
    }
    
    virtual char const *GetError()
    {
        return m_psError;
    }
    
    virtual bool SetProperty( char const *prop, void *value );
    virtual bool GetProperty( char const *prop, void **ptr, bool remove=false );

private:
    ViperPluginType m_type;
    /** Plugin info; name, author, etc */
    viper_plugininfo_t m_info;
    ViperPluginStatus m_status;
    char m_sPath[PLATFORM_MAX_PATH];
    char const *m_sFolder;
    
    char const *m_psError;
    char const *m_psErrorType;
    PyObject *m_pErrorTraceback;
    
    /** The Python thread state of this plugin's sub-interpreter */
    PyThreadState *m_pThreadState;
    /** The Python dictionary used to execute this plug-in */
    PyObject *m_pPluginDict;
    
    Trie *m_pProps;
};

class CPluginManager : public IViperPluginManager, public ViperGlobalClass
{
    friend class CPlugin;

public:
    CPluginManager();
    ~CPluginManager();

public: // ViperGlobalClass
    virtual void OnViperShutdown();

public: // IViperPluginManager
    virtual bool AddPluginsListener(IViperPluginsListener *listener);
    
    /**
     * Loads all the plugin-ins from a directory
     */
    void LoadPluginsFromDir(char const *dir);
    CPlugin *LoadPlugin(char const *path, bool debug, ViperPluginType type,
                        char error[], size_t maxlength, bool *wasloaded);
    bool UnloadPlugin(CPlugin *plugin);
    bool ReloadPlugin(CPlugin *plugin);
    
    /// Returns a plugin from its path
    CPlugin *GetPluginByPath(char const * path);
    /// Returns a plugin from an id
    CPlugin *GetPluginByOrder(int num);
    /// Finds a plugin from a console argument
    CPlugin *FindPluginByConsoleArg(char const *arg);
    /// Returns the number of plugins loaded
    int GetPluginCount();
    
public: // IRootConsoleCommand
    void OnRootConsoleCommand(char const *cmdname, const CCommand &command);
    
private:
    ViperLoadRes _LoadPlugin(CPlugin **_plugin, char const *path, bool debug,
                             ViperPluginType type, char error[], size_t maxlength);
    
private:
    Trie *m_trie;
    SourceHook::List<CPlugin *> m_list;
    
    SourceHook::List<IViperPluginsListener *> m_Listeners;
    
    bool m_LoadingLocked;
};

extern CPluginManager g_VPlugins;

#endif /* _INCLUDE_VIPER_PLUGINSYS_H_ */


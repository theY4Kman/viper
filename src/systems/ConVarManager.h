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

#ifndef _INCLUDE_VIPER_CONVARMANAGER_H_
#define _INCLUDE_VIPER_CONVARMANAGER_H_

#include "viper_globals.h"
#include <compat_wrappers.h>
#include <sh_list.h>
#include <sm_trie.h>
#include <IViperPluginSys.h>
#include <IViperForwardSys.h>
#include "python/py_console.h"

typedef SourceHook::List<const ConVar *> ConVarList;

class ViperConVarManager :
    public ViperGlobalClass,
    public IViperPluginsListener
{
public:
    ViperConVarManager();
    ~ViperConVarManager();
public: // ViperGlobalClass
    virtual void OnViperStartup(bool late);
    virtual void OnViperAllInitialized();
    virtual void OnViperShutdown();
public: // IViperPluginsListener
    virtual void OnPluginUnloaded(IViperPlugin *plugin);
public: // IRootConsoleCommand
    void OnRootConsoleCommand(char const *cmdname, const CCommand &command);
public:
    /**
     * Initializes a new ConVar and adds it to the necessary lists
     *
     * @param pl            The plugin creating the ConVar
     * @param name          Name of the ConVar
     * @param defaultVal    Default value of the ConVar
     * @param desc          Description of the ConVar
     * @param flags         ConVar flags
     * @param hasMin        Boolean stating whether or not the ConVar has a minimum value
     * @param min           A float containing the minimum value of the ConVar
     * @param hasMax        Boolean stating whether or not the ConVar has a maximum value
     * @param max           A float containing the maximum value of the ConVar
     */

    console__ConVar *CreateConVar(IViperPlugin *pl, char const *name,
                                  char const *defaultVal, char const *desc,
                                  int flags, bool hasMin, float min,
                                  bool hasMax, float max);
    
    void HookConVarChange(ConVar *pVar, IViperPluginFunction *pFunc);
    void UnhookConVarChange(ConVar *pVar, IViperPluginFunction *pFunc);
    
private:
    static void AddConVarToPluginList(IViperPlugin *pl, ConVar *cvar);
    
#if SOURCE_ENGINE >= SE_ORANGEBOX
    void OnConVarChanged(ConVar *pConVar, char const *oldValue, float flOldValue);
#else
    void OnConVarChanged(ConVar *pConVar, char const *oldValue);
#endif
private:
    PyObject *m_HookParams;
    Trie *m_ConVarCache;
    SourceHook::List<console__ConVar *> m_ConVars;
};

extern ViperConVarManager g_ConVarManager;

#endif /* _INCLUDE_VIPER_CONVARMANAGER_H_ */

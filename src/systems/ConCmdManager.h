/**
 * =============================================================================
 * Viper
 * Copyright (C) 2010 Brandon "monokrome" Stoner
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
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

#ifndef _VIPER_INCLUDE_CONCMDMANAGER_H_
#define _VIPER_INCLUDE_CONCMDMANAGER_H_

#include "viper_globals.h"
#include <sm_trie_tpl.h>
#include <sh_list.h>
#include "IViperPluginSys.h"
#include <compat_wrappers.h>

enum CmdType
{
    Cmd_Server = 0,
    Cmd_Console,
    Cmd_Admin
};

struct AdminCmdInfo
{
    AdminCmdInfo()
    {
        cmdGrpId = -1;
        flags = 0;
        eflags = 0;
    };

    int cmdGrpId;           /* index into cmdgroup string table */
    SourceMod::FlagBits flags;         /* default flags */
    SourceMod::FlagBits eflags;        /* effective flags */
};

struct CmdHook
{
    CmdHook() : pf(NULL), pAdmin(NULL) {};
    
    PyFunction *pf;         /* function hook */
    char const *helptext;   /* help text */
    AdminCmdInfo *pAdmin;   /* admin requirements, if any */
    IViperPlugin *pl;
};

struct ConCmdInfo
{
    ConCmdInfo() : pCmd(NULL), byViper(false), is_admin_set(false) {};
    
    /// Server callbacks
    SourceHook::List<CmdHook*> srvhooks;
    /// Console callbacks
    SourceHook::List<CmdHook*> conhooks;
    ConCommand *pCmd;
    IViperPlugin *pl;
    /// Was the ConCmd created by Viper?
    bool byViper;
    bool is_admin_set;
};

struct PlCmdInfo
{
    ConCmdInfo *pInfo;
    CmdHook *pHook;
    CmdType cmdType;
};

typedef SourceHook::List<PlCmdInfo> CmdList;
typedef SourceHook::List<ConCommand*> ConCmdList;

class CConCmdManager :
    public SourceMod::IRootConsoleCommand,
    public ViperGlobalClass,
    public IViperPluginsListener
{
#if SOURCE_ENGINE >= SE_ORANGEBOX
    friend void CommandCallback(const CCommand &command);
#else
    friend void CommandCallback();
#endif

public: // IRootConsoleCommand
    void OnRootConsoleCommand(char const *cmdname, const CCommand &command);
public: // ViperGlobalClass
    virtual void OnViperAllInitialized();
    virtual void OnViperShutdown();
public: // IViperPluginsListener
    virtual void OnPluginUnloaded(IViperPlugin *plugin);

public:
    bool AddCommand(IViperPlugin *pPlugin, PyFunction *callback, CmdType type,
        char const *name, char const *description, int flags);

    /**
     * Adds a ConCommand to the ConCmdManager's trie
     */
    void AddToCmdList(ConCmdInfo *pInfo);
        
    void SetCommandClient(int client);
    int GetCommandClient();

    void RemoveConCmd(ConCmdInfo *pInfo, char const *name, bool is_read_safe);
    void RemoveConCmds(SourceHook::List<CmdHook *> &cmdlist, IViperPlugin *pl);
    
    ViperResultType DispatchClientCommand(int client, const char *cmd, int args, ViperResultType type);

private:
    /** Adds a new command or finds one that already exists */
    ConCmdInfo * AddOrFindCommand(char const *name, char const *description, int flags);
    void InternalDispatch(const CCommand &command);

private:
    KTrie<ConCmdInfo*> m_pCmds;
    SourceHook::List<ConCmdInfo *> m_CmdList;
    int m_CmdClient;
};

extern CConCmdManager g_VCmds;

#endif /* _VIPER_INCLUDE_CONCMDMANAGER_H_ */


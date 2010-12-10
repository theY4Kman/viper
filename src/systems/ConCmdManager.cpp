/**
 * =============================================================================
 * Viper
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

#include "systems/ConCmdManager.h"
#include "python/py_console.h"
#include "viper.h"
#include "console.h"
#include "IViperForwardSys.h"
#include "PluginSys.h"
#include "PlayerManager.h"
#include <assert.h>

#if SOURCE_ENGINE >= SE_ORANGEBOX
    SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand &);
#else
    SH_DECL_HOOK0_void(ConCommand, Dispatch, SH_NOATTRIB, false);
#endif

#if SOURCE_ENGINE >= SE_ORANGEBOX
void CommandCallback(const CCommand &command)
{
#else
void CommandCallback()
{
    CCommand command;
#endif
    g_Viper.PushCommandStack(&command);
    
    g_VCmds.InternalDispatch(command);
    
    g_VConsole.CommandCallback(command);
    
    g_Viper.PopCommandStack();
}

void
AddToPlCmdList(CmdList *pList, const PlCmdInfo &info)
{
    CmdList::iterator iter = pList->begin();
    char const *orig = NULL;
    
    orig = info.pInfo->pCmd->GetName();
    
    /* Insert this into the help list, SORTED alphabetically. */
    while (iter != pList->end())
    {
        PlCmdInfo &obj = (*iter);
        char const *cmd = obj.pInfo->pCmd->GetName();
        
        int cmp = strcmp(orig, cmd);
        if (cmp < 0)
        {
            pList->insert(iter, info);
            return;
        }
        /* Don't add duplicates */
        else if (cmp == 0)
            return;
        
        iter++;
    }
    
    pList->push_back(info);
}

CConCmdManager::CConCmdManager() : m_CmdClient(0)
{
}

void
CConCmdManager::OnViperAllInitialized()
{
    g_VPlugins.AddPluginsListener((IViperPluginsListener*)this);
}

void
CConCmdManager::OnViperShutdown()
{
    SourceHook::List<ConCmdInfo *>::iterator iter;
    for (iter=m_CmdList.begin(); iter!=m_CmdList.end(); iter++)
    {
        if ((*iter) == NULL)
            continue;
        
        RemoveConCmd((*iter), (*iter)->pCmd->GetName(), true);
    }
}

void
CConCmdManager::OnPluginUnloaded(IViperPlugin *plugin)
{
    CmdList *pList;
    SourceHook::List<ConCmdInfo *> removed;
    
    if (!plugin->GetProperty("CommandList", (void **)&pList, true) ||
        pList == NULL)
        return;
    
    CmdList::iterator iter;
    for (iter=pList->begin(); iter!=pList->end(); iter++)
    {
        PlCmdInfo &cmd = (*iter);
        ConCmdInfo *pInfo = cmd.pInfo;
        
        /* Has this chain already been fully cleaned/removed? */
        if (removed.find(pInfo) != removed.end())
            continue;
        
        /* Remove any hooks from us on this command */
        RemoveConCmds(pInfo->srvhooks, plugin);
        RemoveConCmds(pInfo->conhooks, plugin);
        
        /* See if there are still hooks */
        if (pInfo->srvhooks.size())
            continue;
        if (pInfo->conhooks.size())
            continue;
        
        /* Remove the command, it should be safe now */
        RemoveConCmd(pInfo, pInfo->pCmd->GetName(), true);
        removed.push_back(pInfo);
    }
    
    delete pList;
}

void
CConCmdManager::OnRootConsoleCommand(char const *cmdname, const CCommand &command)
{
    if (command.ArgC() >= 4)
    {
        char const *text = command.Arg(3);
        
        IViperPlugin *pl = g_VPlugins.FindPluginByConsoleArg(text);
        if (pl == NULL)
        {
            g_pMenu->ConsolePrint("[Viper] Plugin \"%s\" was not found.", text);
            return;
        }
        
        CmdList *pList;
        if (!pl->GetProperty("CommandList", (void **)&pList) || !pList->size())
        {
            g_pMenu->ConsolePrint("[Viper] No commands found for \"%s\"", pl->GetName());
            return;
        }
        
        CmdList::iterator iter;
        char const *type = NULL;
        char const *help;
        
        g_pMenu->ConsolePrint("[Viper] Listing %d command%s for: %s",
            pList->size(), pList->size() != 1 ? "s" : "", pl->GetName());
        g_pMenu->ConsolePrint("  %-17.16s %-12.11s %s", "[Name]", "[Type]", "[Help]");
        for (iter=pList->begin();
             iter!=pList->end();
             iter++)
        {
            PlCmdInfo &cmd = (*iter);
            
            switch(cmd.cmdType)
            {
            case Cmd_Server:
                type = "server";
                break;
            case Cmd_Console:
                type = "console";
                break;
            case Cmd_Admin:
                type = "admin";
                break;
            default:
                /* 'sawce' is not a valid command type. */
                assert(false);
                break;
            }
            
            help = (cmd.pHook->helptext != NULL) ? cmd.pHook->helptext :
                cmd.pInfo->pCmd->GetHelpText();
            
            g_pMenu->ConsolePrint("  %-17.16s %-12.11s %s",
                cmd.pInfo->pCmd->GetName(), type, help);
        }

        return;
    }

    g_pMenu->ConsolePrint("[Viper] Usage: sm py cmds <#|name>");
}

bool
CConCmdManager::AddCommand(IViperPlugin *pPlugin, PyFunction *callback, CmdType type,
                           char const *name, char const *description, int flags)
{
    if (pPlugin == NULL || callback == NULL)
        return false;
    
    ConCmdInfo *pInfo = AddOrFindCommand(name, description, flags);
    
    if (pInfo == NULL)
        return false;
    
    pInfo->pl = pPlugin;
    
    Py_INCREF(callback);
    CmdHook *pHook = new CmdHook();
    pHook->pl = pPlugin;
    pHook->pf = callback;
    
    if (IS_STR_FILLED(description))
        pHook->helptext = sm_strdup(description);
    else
        pHook->helptext = NULL;
    
    if (type == Cmd_Console || type == Cmd_Admin)
        pInfo->conhooks.push_back(pHook);
    else
        pInfo->srvhooks.push_back(pHook);
    
    /* Add cmd to the plugin list */
    CmdList *pList;
    if (!pPlugin->GetProperty("CommandList", (void **)&pList))
    {
        pList = new CmdList();
        pPlugin->SetProperty("CommandList", pList);
    }
    
    PlCmdInfo info;
    info.pInfo = pInfo;
    info.cmdType = type;
    info.pHook = pHook;
    
    AddToPlCmdList(pList, info);
    
    return true;
}

void
CConCmdManager::SetCommandClient(int client)
{
    m_CmdClient = client + 1;
}

int
CConCmdManager::GetCommandClient()
{
    return m_CmdClient;
}

void
CConCmdManager::RemoveConCmd(ConCmdInfo *pInfo, char const *name, bool is_read_safe)
{
    m_pCmds.remove(name);
    
    if(pInfo->pCmd != NULL)
    {
        if(pInfo->byViper)
        {
            /* Unlink from SourceMM */
            META_UNREGCVAR(pInfo->pCmd);
            
            /* Delete the command's memory */
            delete [] const_cast<char *>(pInfo->pCmd->GetHelpText());
            delete [] const_cast<char *>(pInfo->pCmd->GetName());
            
            delete pInfo->pCmd;
        }
        /* Remove the external hook */
        else if(is_read_safe)
        {
            SH_REMOVE_HOOK_STATICFUNC(ConCommand, Dispatch, pInfo->pCmd,
                CommandCallback, false);
        }
    }
    
    m_CmdList.remove(pInfo);
    
    delete pInfo;
}

void
CConCmdManager::RemoveConCmds(SourceHook::List<CmdHook *> &cmdlist, IViperPlugin *pl)
{
    SourceHook::List<CmdHook *>::iterator iter = cmdlist.begin();
    CmdHook *pHook;
    
    while (iter != cmdlist.end())
    {
        pHook = (*iter);
        if (pHook->pl != pl)
        {
            iter++;
            continue;
        }
        
        delete pHook->pAdmin;
        
        Py_DECREF(pHook->pf);
        delete pHook;
        iter = cmdlist.erase(iter);
    }
}

ConCmdInfo *
CConCmdManager::AddOrFindCommand(char const *name, char const *description,
                                 int flags)
{
    ConCmdInfo *pInfo;
    ConCmdInfo **val = m_pCmds.retrieve(name);

    // Aww, darn, sawce! You stole it from the trie!
    if(val == NULL)
        pInfo = NULL;
    else
        return *val;

    /* Don't you just love it when you take code directly from SourceMod?
     * If you do, you'll love this next section!
     */
    if(pInfo == NULL)
    {
        ConCommandBase *pBase = FindCommandBase(name);
        ConCommand *pCmd = NULL;
        if (pBase != NULL)
        {
            // Don't create a command with the same name as a convar
            if (!pBase->IsCommand())
                return NULL;
            
            pCmd = static_cast<ConCommand*>(pBase);
        }
        
        // Make sure we have a valid command before we build a new ConCmdInfo
        pInfo = new ConCmdInfo();

        if (pCmd == NULL)
        {
            /* Note that we have to duplicate because the source might not be 
             * a static string, and these expect static memory.
             */
            if (description == NULL)
                description = "";
            
            char *new_name = sm_strdup(name);
            char *new_help = sm_strdup(description);
            pCmd = new ConCommand(new_name, CommandCallback, new_help, flags);

            pInfo->byViper = true;
        }
        else
            SH_ADD_HOOK_STATICFUNC(ConCommand, Dispatch, pCmd, CommandCallback, false);

        pInfo->pCmd = pCmd;
        pInfo->is_admin_set = false;

        m_pCmds.insert(name, pInfo);
    }

    return pInfo;
}

void
CConCmdManager::InternalDispatch(const CCommand &command)
{
    int client = m_CmdClient;
    char const *cmd = command.Arg(0);

    ConCmdInfo **temp = m_pCmds.retrieve(cmd);
    ConCmdInfo *pInfo;
    if (temp != NULL)
        pInfo = (*temp);
    
    if (temp == NULL || pInfo == NULL)
    {
        /* Unfortunately, we now have to do a slow lookup because Valve made client commands 
         * case-insensitive.  We can't even use our sortedness.
         */
        if (client == 0 && !engine->IsDedicatedServer())
            return;

        SourceHook::List<ConCmdInfo *>::iterator iter;

        pInfo = NULL;
        iter = m_CmdList.begin();
        while (iter != m_CmdList.end())
        {
            if (strcasecmp((*iter)->pCmd->GetName(), cmd) == 0)
            {
                pInfo = (*iter);
                break;
            }
            iter++;
        }

        if (pInfo == NULL)
            return;
    }
    
    ViperResultType result = Pl_Continue;
    SourceHook::List<CmdHook *>::iterator iter;
    CmdHook *pHook;
    
    PyThreadState *_save = PyThreadState_Get();
    
    /* Build the sourcemod.console.ConCommand object */
    int args = command.ArgC();
    PyObject *args_list = PyList_New(args-1);
    
    int i;
    for (i=1; i<args; i++)
        PyList_SetItem(args_list, i-1, PyString_FromString(command.Arg(i)));
    
    /* Creates a new ConCommandReply object */
    console__ConCommandReply *py_cmd = PyObject_New(console__ConCommandReply,
        &console__ConCommandReplyType);
    
    py_cmd->args = (PyListObject*)args_list;
    py_cmd->argstring = command.ArgS();
    py_cmd->name = cmd;
    py_cmd->reply_to = playerhelpers->GetReplyTo();
    py_cmd->client = g_Players.GetPythonClient(client);
    assert(py_cmd->client != NULL);
    
    PyObject *argslist = PyTuple_Pack(1, (PyObject*)py_cmd);
    
    if (client == 0 && pInfo->srvhooks.size())
    {
        ViperResultType tempres = result;
        for (iter=pInfo->srvhooks.begin();
             iter!=pInfo->srvhooks.end();
             iter++)
        {
            pHook = (*iter);
            
            if(!PyCallable_Check(pHook->pf))
                continue;
            
            PyThreadState_Swap(pInfo->pl->GetThreadState());
            
            /* Goddamn it. Here's a bug. */
            /* (01-19-09): What in the hell did I mean by that? */
            PyObject *pyres = NULL;
            pyres = PyObject_CallObject(pHook->pf, argslist);
            
            if(pyres == NULL)
            {
                if (PyErr_Occurred())
                    PyErr_Print();
                
                PyThreadState_Swap(_save);
                continue;
            }
            
            if(pyres == Py_None || !PyInt_Check(pyres))
            {
                Py_DECREF(pyres);

                PyThreadState_Swap(_save);
                continue;
            }
            
            tempres = (ViperResultType)PyInt_AsLong(pyres);
            Py_DECREF(pyres);

            if(tempres > result)
                result = tempres;
            if(result == Pl_Stop)
            {
                PyThreadState_Swap(_save);
                break;
            }
        }
    
        /* Check if there's an early stop */
        if (result >= Pl_Stop)
        {
            if (!pInfo->byViper)
                RETURN_META(MRES_SUPERCEDE);
            
            return;
        }
    }

    if (pInfo->conhooks.size())
    {
        ViperResultType tempres = result;
        for (iter = pInfo->conhooks.begin();
            iter != pInfo->conhooks.end();
            iter++)
        {
            pHook = (*iter);

            if(!PyCallable_Check(pHook->pf))
                continue;

#if SAWCE != RUKIA
            /**
             * TODO: Admin commands
             */
            if (client 
                && pHook->pAdmin
                && !CheckAccess(client, cmd, pHook->pAdmin))
            {
                if (result < Pl_Handled)
                {
                    result = Pl_Handled;
                }
                continue;
            }

            /* On a listen server, sometimes the server host's client index can be set as 0.
             * So index 1 is passed to the command callback to correct this potential problem.
             * 
             * TODO: Write in support for this
             */
            if (!engine->IsDedicatedServer())
                client = g_Players.ListenClient();
#endif

            PyThreadState_Swap(pInfo->pl->GetThreadState());
            
            PyObject *pyres = PyObject_CallObject(pHook->pf, argslist);

            if(pyres == NULL)
            {
                if (PyErr_Occurred())
                    PyErr_Print();
                
                PyThreadState_Swap(_save);
                continue;
            }
            
            if(pyres == Py_None || !PyInt_Check(pyres))
            {
                Py_DECREF(pyres);

                PyThreadState_Swap(_save);
                continue;
            }
            
            tempres = (ViperResultType)PyInt_AsLong(pyres);
            Py_DECREF(pyres);

            if(tempres > result)
                result = tempres;
            if(result == Pl_Stop)
            {
                PyThreadState_Swap(_save);
                break;
            }
        }
    }
    
    Py_DECREF(argslist);

    if (result >= Pl_Handled)
    {
        if (!pInfo->byViper)
            RETURN_META(MRES_SUPERCEDE);
    }
}

ViperResultType
CConCmdManager::DispatchClientCommand(int client, const char *cmd, int args, ViperResultType type)
{
    ConCmdInfo **temp = m_pCmds.retrieve(cmd);
    ConCmdInfo *pInfo;
    if (temp != NULL)
        pInfo = (*temp);
    
    SetCommandClient(client);
    
    if (temp == NULL || pInfo == NULL)
    {
        /* Unfortunately, we now have to do a slow lookup because Valve made client commands 
         * case-insensitive.  We can't even use our sortedness.
         */
        if (client == 0 && !engine->IsDedicatedServer())
            return type;

        SourceHook::List<ConCmdInfo *>::iterator iter;

        pInfo = NULL;
        iter = m_CmdList.begin();
        while (iter != m_CmdList.end())
        {
            if (strcasecmp((*iter)->pCmd->GetName(), cmd) == 0)
            {
                pInfo = (*iter);
                break;
            }
            iter++;
        }

        if (pInfo == NULL)
            return type;
    }
    
    ViperResultType result = Pl_Continue;
    SourceHook::List<CmdHook *>::iterator iter;
    CmdHook *pHook;
    
    PyThreadState *_save = PyThreadState_Get();
    
    /* Build the sourcemod.console.ConCommand object */
    PyObject *args_list = PyList_New(args-1);
    
    const CCommand *command = g_Viper.PeekCommandStack();
    int i;
    for (i=1; i<args; i++)
        PyList_SetItem(args_list, i-1, PyString_FromString(command->Arg(i)));
    
    /* Creates a new ConCommandReply object */
    console__ConCommandReply *py_cmd = PyObject_New(console__ConCommandReply,
        &console__ConCommandReplyType);
    
    py_cmd->args = (PyListObject*)args_list;
    py_cmd->argstring = command->ArgS();
    py_cmd->name = cmd;
    py_cmd->client = g_Players.GetPythonClient(client);
    assert(py_cmd->client != NULL);
    
    PyObject *argslist = PyTuple_Pack(1, (PyObject*)py_cmd);
    
    if (pInfo->conhooks.size())
    {
        ViperResultType tempres = result;
        for (iter = pInfo->conhooks.begin();
            iter != pInfo->conhooks.end();
            iter++)
        {
            pHook = (*iter);

            if(!PyCallable_Check(pHook->pf))
                continue;

#if SAWCE != RUKIA
            /**
             * TODO: Admin commands
             */
            if (client 
                && pHook->pAdmin
                && !CheckAccess(client, cmd, pHook->pAdmin))
            {
                if (result < Pl_Handled)
                {
                    result = Pl_Handled;
                }
                continue;
            }

            /* On a listen server, sometimes the server host's client index can be set as 0.
             * So index 1 is passed to the command callback to correct this potential problem.
             * 
             * TODO: Write in support for this
             */
            if (!engine->IsDedicatedServer())
                client = g_Players.ListenClient();
#endif

            PyThreadState_Swap(pInfo->pl->GetThreadState());
            
            PyObject *pyres = PyObject_CallObject(pHook->pf, argslist);

            if(pyres == NULL)
            {
                if (PyErr_Occurred())
                    PyErr_Print();
                
                PyThreadState_Swap(_save);
                continue;
            }
            
            if(pyres == Py_None || !PyInt_Check(pyres))
            {
                Py_DECREF(pyres);

                PyThreadState_Swap(_save);
                continue;
            }
            
            tempres = (ViperResultType)PyInt_AsLong(pyres);
            Py_DECREF(pyres);

            if(tempres > result)
                result = tempres;
            if(result == Pl_Stop)
            {
                PyThreadState_Swap(_save);
                break;
            }
        }
    }
    
    Py_DECREF(argslist);
    
    type = result;
    
    return type;
}

CConCmdManager g_VCmds;


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

#include "ConVarManager.h"
#include "PluginSys.h"
#include "ForwardSys.h"

ViperConVarManager::ViperConVarManager()
{
    m_ConVarCache = sm_trie_create();
}

ViperConVarManager::~ViperConVarManager()
{
    sm_trie_destroy(m_ConVarCache);
}

void
ViperConVarManager::OnRootConsoleCommand(char const *cmdname,
                                         const CCommand &command)
{
    if (command.ArgC() >= 4)
    {
        char const *plstr = command.Arg(3);
        IViperPlugin *pl = g_VPlugins.FindPluginByConsoleArg(plstr);

        if (pl == NULL)
        {
            g_pMenu->ConsolePrint("[Viper] Plugin '%s' not found.", plstr);
            return;
        }

        ConVarList *cvars;
        if (!pl->GetProperty("ConVarList", (void**)&cvars))
        {
            g_pMenu->ConsolePrint("[Viper] Plugin '%s' has not registered any convars.", plstr);
            return;
        }

        g_pMenu->ConsolePrint("[Viper] Listing %d convars for plugin '%s':", cvars->size(), plstr);
        g_pMenu->ConsolePrint("  %-32.31s %s", "[Name]", "[Value]");

        ConVarList::iterator iter;
        for(iter = cvars->begin(); iter != cvars->end(); iter++)
        {
            const ConVar *cvar = (*iter);
            g_pMenu->ConsolePrint("  %-32.31s %s", cvar->GetName(), cvar->GetString()); 
        }

        return;
    }

    g_pMenu->ConsolePrint("[Viper] Usage: sm py cvars <#>");
}

console__ConVar *
ViperConVarManager::CreateConVar(IViperPlugin *pl, char const *name,
                                 char const *defaultVal, char const *desc,
                                 int flags, bool hasMin, float min,
                                 bool hasMax, float max)
{
    ConVar *cvar;
    console__ConVar *handle;
    
    cvar = icvar->FindVar(name);
    if (cvar != NULL)
    {
        AddConVarToPluginList(pl, cvar);
        
        if (sm_trie_retrieve(m_ConVarCache, name, (void**)&handle))
            return handle;
        
        handle = PyObject_GC_New(console__ConVar, &console__ConVarType);
        handle->byViper = false;
        handle->cvarChangeHooks = NULL;
        handle->pVar = cvar;
        
        m_ConVars.push_back(handle);
        sm_trie_insert(m_ConVarCache, name, handle);
        
        return handle;
    }
    
    ConCommandBase *pBase = icvar->GetCommands();
    while (pBase != NULL)
    {
        if (pBase->IsCommand() && strcmp(pBase->GetName(), name) == 0)
            return NULL;
        
        pBase = const_cast<ConCommandBase *>(pBase->GetNext());
    }
    
    handle = PyObject_GC_New(console__ConVar, &console__ConVarType);
    handle->byViper = true;
    handle->cvarChangeHooks = NULL;
    
    cvar = new ConVar(sm_strdup(name), sm_strdup(defaultVal), flags,
        sm_strdup(desc), hasMin, min, hasMax, max);
    META_REGCVAR(cvar);
    handle->pVar = cvar;
    
    AddConVarToPluginList(pl, cvar);
    sm_trie_insert(m_ConVarCache, name, handle);
    
    return handle;
}

void
ViperConVarManager::OnPluginUnloaded(IViperPlugin *pl)
{
    /* TODO */
}

void
ViperConVarManager::AddConVarToPluginList(IViperPlugin *pl, ConVar *cvar)
{
    ConVarList *pConVarList;
    if (!pl->GetProperty("ConVarList", (void**)&pConVarList))
    {
        pConVarList = new ConVarList();
        pl->SetProperty("ConVarList", pConVarList);
    }
    else if (pConVarList->find(cvar) != pConVarList->end())
    {
        /* Don't add what's already in the list */
        return;
    }
    
    ConVarList::iterator iter;
    char const *name = cvar->GetName();
    
    /* Insert ConVar into the alphabetically sorted list */
    for (iter=pConVarList->begin(); iter!=pConVarList->end(); iter++)
    {
        if (strcmp(name, (*iter)->GetName()) < 0)
        {
            pConVarList->insert(iter, cvar);
            return;
        }
    }
    
    /* If the cvar was not inserted, add it to the end of the list */
    pConVarList->push_back(cvar);
}

void
ViperConVarManager::HookConVarChange(ConVar *pVar, IViperPluginFunction *pFunc)
{
    console__ConVar *handle;
    IViperForward *pForward;
    
    if (!sm_trie_retrieve(m_ConVarCache, pVar->GetName(), (void**)&handle))
        return;
    
    pForward = handle->cvarChangeHooks;
    
    /* Create the forward if it does not exist already */
    if (pForward == NULL)
    {
        PyObject *types = PyTuple_Pack(3, (PyObject*)&console__ConVarType,
            PyString_Type, PyString_Type);
        
        pForward = g_Forwards.CreateForward("", ET_Ignore, types, NULL);
        handle->cvarChangeHooks = pForward;
    }
    
    pForward->AddFunction(pFunc);
}

void
ViperConVarManager::UnhookConVarChange(ConVar *pVar, IViperPluginFunction *pFunc)
{
    console__ConVar *handle;
    IViperForward *pForward;
    
    if (!sm_trie_retrieve(m_ConVarCache, pVar->GetName(), (void**)&handle))
        return;
    
    pForward = handle->cvarChangeHooks;
    if (pForward == NULL)
    {
        PyErr_SetString(g_pViperException, "ConVar has no active hook.");
        return;
    }
    
    if (!pForward->RemoveFunction(pFunc))
    {
        PyErr_SetString(g_pViperException, "Invalid hook callback passed.");
        return;
    }
    
    if (pForward->GetFunctionCount() == 0)
    {
        g_Forwards.ReleaseForward(pForward);
        handle->cvarChangeHooks = NULL;
    }
    
    return;
}

ViperConVarManager g_ConVarManager;


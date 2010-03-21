/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
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

#if SOURCE_ENGINE < SE_ORANGEBOX
#define CallGlobalChangeCallbacks  CallGlobalChangeCallback
#endif

#if SOURCE_ENGINE >= SE_ORANGEBOX
SH_DECL_HOOK3_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false, ConVar *, const char *, float);
#else
SH_DECL_HOOK2_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false, ConVar *, const char *);
#endif

ViperConVarManager::ViperConVarManager()
{
    m_ConVarCache = sm_trie_create();
}

ViperConVarManager::~ViperConVarManager()
{
    sm_trie_destroy(m_ConVarCache);
}

void
ViperConVarManager::OnViperStartup(bool late)
{
    m_HookParams = PyTuple_Pack(3, &console__ConVarType, _PyString_Type,
                                _PyString_Type);
    assert(m_HookParams != NULL);
}

void
ViperConVarManager::OnViperAllInitialized()
{
    SH_ADD_HOOK_MEMFUNC(ICvar, CallGlobalChangeCallbacks, icvar, this,
        &ViperConVarManager::OnConVarChanged, false);
}

void
ViperConVarManager::OnViperShutdown()
{
    SourceHook::List<console__ConVar *>::iterator iter = m_ConVars.begin();
    while (iter != m_ConVars.end())
    {
        console__ConVar *handle = (*iter);
        
        iter = m_ConVars.erase(iter);
        
        if (handle->cvarChangeHooks != NULL)
            g_Forwards.ReleaseForward(handle->cvarChangeHooks);
        if (handle->byViper)
        {
            META_UNREGCVAR(handle->pVar);
            delete [] handle->pVar->GetName();
            delete [] handle->pVar->GetHelpText();
            delete [] handle->pVar->GetDefault();
            delete handle->pVar;
            
            handle->pVar = NULL;
        }
        
        Py_DECREF((PyObject *)handle);
    }
    
    sm_trie_clear(m_ConVarCache);
    
    Py_DECREF(m_HookParams);
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
        
        handle = PyObject_New(console__ConVar, &console__ConVarType);
        assert(handle != NULL);
        
        handle->byViper = false;
        handle->name = cvar->GetName();
        handle->cvarChangeHooks = NULL;
        handle->pVar = cvar;
        
        Py_INCREF(handle);
        m_ConVars.push_back(handle);
        sm_trie_insert(m_ConVarCache, name, handle);
        
        return handle;
    }
    
    if (FindCommand(name) != NULL)
        return NULL;
    
    handle = PyObject_New(console__ConVar, &console__ConVarType);
    assert(handle != NULL);
    
    handle->byViper = true;
    handle->cvarChangeHooks = NULL;
    
    cvar = new ConVar(sm_strdup(name), sm_strdup(defaultVal), flags,
        sm_strdup(desc), hasMin, min, hasMax, max);
    
    handle->pVar = cvar;
    handle->name = cvar->GetName();
    
    AddConVarToPluginList(pl, cvar);
    
    Py_INCREF(handle);
    sm_trie_insert(m_ConVarCache, name, handle);
    
    return handle;
}

PyObject *
ViperConVarManager::FindConVar(char const *name)
{
    ConVar *cvar;
    console__ConVar *handle;
    
    cvar = icvar->FindVar(name);
    if (cvar == NULL)
        Py_RETURN_NONE;
    
    if (sm_trie_retrieve(m_ConVarCache, name, (void**)&handle))
        return (PyObject *)handle;
    
    handle = PyObject_New(console__ConVar, &console__ConVarType);
    assert(handle != NULL);
    
    handle->byViper = false;
    handle->name = cvar->GetName();
    handle->cvarChangeHooks = NULL;
    handle->pVar = cvar;
    
    Py_INCREF(handle);
    m_ConVars.push_back(handle);
    sm_trie_insert(m_ConVarCache, name, handle);
    
    return (PyObject *)handle;
}

void
ViperConVarManager::OnPluginUnloaded(IViperPlugin *pl)
{
    ConVarList *pConVarList;
    if (!pl->GetProperty("ConVarList", (void**)&pConVarList, true))
        return;
    
    pConVarList->clear();
    delete pConVarList;
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
        pForward = g_Forwards.CreateForward("", ET_Ignore, m_HookParams, NULL);
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

void
#if SOURCE_ENGINE >= SE_ORANGEBOX
ViperConVarManager::OnConVarChanged(ConVar *pConVar, char const *oldValue,
                                    float flOldValue)
#else
ViperConVarManager::OnConVarChanged(ConVar *pConVar, char const *oldValue)
#endif
{
    /* If the values are the same, exit early in order to not trigger callbacks */
    if (strcmp(pConVar->GetString(), oldValue) == 0)
        return;
    
    console__ConVar *handle;
    
    /* Find the convar in the lookup trie */
    if (!sm_trie_retrieve(m_ConVarCache, pConVar->GetName(), (void**)&handle))
        return;
    
    IViperForward *pForward = handle->cvarChangeHooks;
    if (pForward != NULL)
    {
        PyObject *args = Py_BuildValue("(Oss)", handle, oldValue,
            handle->pVar->GetString());
        pForward->Execute(NULL, args);
        
        Py_DECREF(args);
    }
}

ViperConVarManager g_ConVarManager;


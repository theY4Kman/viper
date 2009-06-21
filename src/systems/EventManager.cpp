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

#include "EventManager.h"
#include "ForwardSys.h"
#include "python/py_events.h"

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *,
    bool);
typedef SourceHook::List<ViperEventHook *> ViperEventHookList;

ViperEventManager::ViperEventManager()
{
    m_EventHooks = sm_trie_create();
}

ViperEventManager::~ViperEventManager()
{
    sm_trie_destroy(m_EventHooks);
}

void
ViperEventManager::OnViperStartup(bool late)
{
    m_HookParams = PyTuple_Pack(2, &events__EventType, &PyString_Type);
    Py_INCREF(m_HookParams);
}

void
ViperEventManager::OnViperAllInitialized()
{
    SH_ADD_HOOK_MEMFUNC(IGameEventManager2, FireEvent, gameevents, this,
        &ViperEventManager::OnFireEvent, false);
    SH_ADD_HOOK_MEMFUNC(IGameEventManager2, FireEvent, gameevents, this,
        &ViperEventManager::OnFireEvent_Post, true);
}

void
ViperEventManager::OnViperShutdown()
{
    SH_REMOVE_HOOK_MEMFUNC(IGameEventManager2, FireEvent, gameevents, this,
        &ViperEventManager::OnFireEvent, false);
    SH_REMOVE_HOOK_MEMFUNC(IGameEventManager2, FireEvent, gameevents, this,
        &ViperEventManager::OnFireEvent_Post, true);
    
    gameevents->RemoveListener(this);
}

void
ViperEventManager::OnPluginUnloaded(IViperPlugin *pl)
{
    ViperEventHookList *pHookList;
    ViperEventHookList::iterator iter;
    ViperEventHook *pHook;

    if (!pl->GetProperty("EventHooks", (void**)&pHookList, true))
        return;
    
    for (iter = pHookList->begin(); iter != pHookList->end(); iter++)
    {
        pHook = (*iter);

        if (--pHook->refcnt != 0)
            continue;
        
        if (pHook->pPreHook != NULL)
            g_Forwards.ReleaseForward(pHook->pPreHook);

        if (pHook->pPostHook != NULL)
            g_Forwards.ReleaseForward(pHook->pPostHook);

        delete pHook;
    }

    delete pHookList;
}

void
ViperEventManager::FireGameEvent(IGameEvent *pEvent)
{
	/* No need to do anything here.
	 * Just need to add ourselves as a listener to make our hook on
	 * IGameEventManager2::FireEvent work
	 */
}

ViperEventHookError
ViperEventManager::HookEvent(char const *name, IViperPluginFunction *pFunc,
                             ViperEventHookMode mode)
{
    if (!gameevents->FindListener(this, name))
    {
        if (!gameevents->AddListener(this, name, true))
        {
            /* sawce was too lazy to create this event. It doesn't exist. */
            return EventHookErr_InvalidEvent;
        }
    }
    
    ViperEventHook *pHook;
    
    /* If a hook struct doesn't exist... */
    if (!sm_trie_retrieve(m_EventHooks, name, (void**)&pHook))
    {
        ViperEventHookList *pHookList;
        IViperPlugin *plugin = pFunc->GetOwnerPlugin();
        
        /* Check to see if the plug-in has an existing EventHookList */
        if (!plugin->GetProperty("EventHooks", (void**)&pHookList))
        {
            pHookList = new ViperEventHookList();
            plugin->SetProperty("EventHooks", pHookList);
        }
        
        pHook = new ViperEventHook();
        
        if (mode == EventHookMode_Pre)
        {
            pHook->pPreHook = g_Forwards.CreateForward("", ET_Hook, m_HookParams, NULL);
            pHook->pPreHook->AddFunction(pFunc);
        }
        else
        {
            pHook->pPostHook = g_Forwards.CreateForward("", ET_Ignore, m_HookParams, NULL);
            /* Should we copy data from a pre hook to the post hook? */
            pHook->postCopy = (mode == EventHookMode_Post);
            pHook->pPostHook->AddFunction(pFunc);
        }
        
        /* Cache the name for post hooks */
        pHook->name = sm_strdup(name);
        
        pHook->refcnt++;
        
        pHookList->push_back(pHook);
        sm_trie_insert(m_EventHooks, name, pHook);
        
        return EventHookErr_Okay;
    }
    
    /* The hook structure already exists at this point */
    
    if (mode == EventHookMode_Pre)
    {
        if (pHook->pPostHook == NULL)
            pHook->pPreHook = g_Forwards.CreateForward("", ET_Event, m_HookParams, NULL);
        
        pHook->pPreHook->AddFunction(pFunc);
    }
    else
    {
        if (pHook->pPostHook == NULL)
            pHook->pPostHook = g_Forwards.CreateForward("", ET_Ignore, m_HookParams, NULL);
        
        /* If postCopy is false, then we may want to set it to true */
        if (!pHook->postCopy)
            pHook->postCopy = (mode == EventHookMode_Post);
        
        pHook->pPostHook->AddFunction(pFunc);
    }
    
    pHook->refcnt++;
    
    return EventHookErr_Okay;
}

ViperEventHookError
ViperEventManager::UnhookEvent(char const *name, IViperPluginFunction *pFunc,
                             ViperEventHookMode mode)
{
    ViperEventHook *pHook;
    
    /* If hook doesn't exist at all... */
    if (!sm_trie_retrieve(m_EventHooks, name, (void **)&pHook))
        return EventHookErr_NotActive;
    
    IViperForward **pEventForward;
    if (mode == EventHookMode_Pre)
        pEventForward = &pHook->pPreHook;
    else
        pEventForward = &pHook->pPostHook;
    
    /* Remove function from forward list */
    if (*pEventForward == NULL || !(*pEventForward)->RemoveFunction(pFunc))
        return EventHookErr_InvalidCallback;
    
    /* Release the forward if it contains no hooks; it's just dead weight */
    if ((*pEventForward)->GetFunctionCount() == 0)
    {
        g_Forwards.ReleaseForward(*pEventForward);
        *pEventForward = NULL;
    }
    
    if (--pHook->refcnt == 0)
    {
        ViperEventHookList *pHookList;
        
        if (!pFunc->GetOwnerPlugin()->GetProperty("EventHooks", (void **)&pHookList))
            return EventHookErr_NotActive;
        
        /* Make sure the event was actually being hooked */
        if (pHookList->find(pHook) == pHookList->end())
            return EventHookErr_NotActive;
        
        pHookList->remove(pHook);
        sm_trie_delete(m_EventHooks, name);
        
        delete [] pHook->name;
        delete pHook;
    }
    
    return EventHookErr_Okay;
}

bool
ViperEventManager::OnFireEvent(IGameEvent *pEvent, bool bDontBroadcast)
{
    ViperEventHook *pHook;
    IViperForward *pForward;
    char const *name;
    int res = Pl_Continue;
    
    if (pEvent == NULL)
        RETURN_META_VALUE(MRES_IGNORED, false);
    
    name = pEvent->GetName();
    
    if (sm_trie_retrieve(m_EventHooks, name, (void**)&pHook))
    {
        /* The refcnt is increased to make sure the event struct is not garbage
         * collected between now and the post hook.
         */
        pHook->refcnt++;
        m_EventHookStack.push(pHook);
        
        pForward = pHook->pPreHook;
        
        if (pForward != NULL)
        {
            events__Event *pyEvent = PyObject_GC_New(events__Event,
                &events__EventType);
            pyEvent->event = pEvent;
            pyEvent->bDontBroadcast = bDontBroadcast;
            
            (void)pForward->Execute(&res, PyTuple_Pack(2, pyEvent,
                PyString_FromString(name)));
        }
        
        if (pHook->postCopy)
            m_EventCopies.push(gameevents->DuplicateEvent(pEvent));
        
        if (res)
        {
            gameevents->FreeEvent(pEvent);
            RETURN_META_VALUE(MRES_IGNORED, false);
        }
    }
    else
        m_EventHookStack.push(NULL);
    
    RETURN_META_VALUE(MRES_IGNORED, true);
}

bool
ViperEventManager::OnFireEvent_Post(IGameEvent *pEvent, bool bDontBroadcast)
{
    ViperEventHook *pHook;
    IViperForward *pForward;
    
    if (pEvent == NULL)
        RETURN_META_VALUE(MRES_IGNORED, false);
    
    pHook = m_EventHookStack.front();
    m_EventHookStack.pop();
    
    if (pHook == NULL)
        RETURN_META_VALUE(MRES_IGNORED, false);
    
    pForward = pHook->pPostHook;
    if (pForward != NULL)
    {
        PyObject *args = PyTuple_Pack(2, Py_None, PyString_FromString(pHook->name));
        
        IGameEvent *pEventCopy = m_EventCopies.front();
        if (pHook->postCopy)
        {
            events__Event *pyEvent = PyObject_GC_New(events__Event,
                &events__EventType);
            pyEvent->event = pEventCopy;
            pyEvent->bDontBroadcast = bDontBroadcast;
            
            PyTuple_SetItem(args, 0, (PyObject*)pyEvent);
        }
        
        pForward->Execute(NULL, args);
        
        if (pHook->postCopy)
        {
            gameevents->FreeEvent(pEventCopy);
            m_EventCopies.pop();
        }
    }
    
    if (--pHook->refcnt == 0)
    {
        assert(pHook->pPostHook == NULL);
        assert(pHook->pPreHook == NULL);
        
        sm_trie_delete(m_EventHooks, pHook->name);
        delete [] pHook->name;
        delete pHook;
    }
    
    RETURN_META_VALUE(MRES_IGNORED, true);
}

ViperEventManager g_EventManager;


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

#ifndef _INCLUDE_VIPER_EVENTMANAGER_H_
#define _INCLUDE_VIPER_EVENTMANAGER_H_

#include "viper_globals.h"
#include <igameevents.h>
#include <sm_trie.h>
#include <sh_list.h>
#include <sh_stack.h>
#include <IViperForwardSys.h>
#include <IViperPluginSys.h>

struct ViperEventHook
{
    ViperEventHook()
    {
        pPreHook = NULL;
        pPostHook = NULL;
        postCopy = false;
        refcnt = 0;
    }
    
    IViperForward *pPreHook;
    IViperForward *pPostHook;
    bool postCopy;
    unsigned int refcnt;
    char *name;
};

enum ViperEventHookMode
{
    EventHookMode_Pre,
    EventHookMode_Post,
    EventHookMode_PostNoCopy,
};

enum ViperEventHookError
{
    EventHookErr_Okay = 0,          /**< No error */
    EventHookErr_InvalidEvent,      /**< Event does not exist */
    EventHookErr_NotActive,         /**< Event has no active hook */
    EventHookErr_InvalidCallback,   /**< Event does not fire specified callback */
};

class ViperEventManager :
    public ViperGlobalClass,
    public IViperPluginsListener,
    public IGameEventListener2
{
public:
    ViperEventManager();
    ~ViperEventManager();
public: // ViperGlobalClass
    virtual void OnViperStartup(bool late);
    virtual void OnViperAllInitialized();
    virtual void OnViperShutdown();
public: // IViperPluginsListener
    virtual void OnPluginUnloaded(IViperPlugin *pl);
public: // IGameEventListener2
	void FireGameEvent(IGameEvent *pEvent);
public:
    ViperEventHookError HookEvent(char const *name, IViperPluginFunction *pFunc,
        ViperEventHookMode mode=EventHookMode_Post);
    ViperEventHookError UnhookEvent(char const *name, IViperPluginFunction *pFunc,
        ViperEventHookMode mode=EventHookMode_Post);

private: // IGameEventManager2 hooks
	bool OnFireEvent(IGameEvent *pEvent, bool bDontBroadcast);
	bool OnFireEvent_Post(IGameEvent *pEvent, bool bDontBroadcast);
private:
    PyObject *m_HookParams;
    Trie *m_EventHooks;
    SourceHook::CStack<IGameEvent *> m_EventCopies;
    SourceHook::CStack<ViperEventHook *> m_EventHookStack;
};

extern ViperEventManager g_EventManager;

#endif // _INCLUDE_VIPER_EVENTMANAGER_H_


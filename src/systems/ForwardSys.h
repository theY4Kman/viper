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

#ifndef _INCLUDE_VIPER_FORWARDSYS_H_
#define _INCLUDE_VIPER_FORWARDSYS_H_

#include "viper_globals.h"
#include <IViperForwardSys.h>
#include <IViperPluginSys.h>
#include "PluginSys.h"
#include <sh_list.h>
#include <sh_stack.h>
#include "sm_trie.h"

class CForward : public IViperForward
{
public:
    ~CForward();
public: //IViperForward
    virtual PyObject *Execute(int *result, PyObject *args);
    virtual unsigned int GetFunctionCount();
    virtual char const *GetForwardName();
    virtual bool RemoveFunction(IViperPluginFunction *func);
    virtual bool RemoveFunction(unsigned int idx);
    virtual void RemoveFunctionsOfPlugin(IViperPlugin *plugin);
    
    /**
     * @note: The PluginFunction passed is copied and saved, so you're free to
     *        delete it after calling this function.
     */
    virtual void AddFunction(IViperPluginFunction *func);
    
    virtual IViperPluginFunction *GetFunction(unsigned int idx);
    
    virtual ViperExecType GetExecType();
    virtual PyObject *GetParamTypes();
    virtual void Clear();
public:
    static CForward *CreateForward(char const *name, ViperExecType et,
                                   PyObject *types, IViperForwardCallback callback);

protected:
    mutable SourceHook::List<IViperPluginFunction *> m_functions;
    mutable SourceHook::List<IViperPluginFunction *> m_paused;
    
    ViperExecType m_ExecType;
    char const *m_sName;
    IViperForwardCallback m_callback;
    PyObject *m_types;
    int m_iTypesCnt;
};

class CForwardManager :
    public ViperGlobalClass,
    public IViperForwardManager,
    public IViperPluginsListener
{
    friend class CForward;
public:
    CForwardManager();
    ~CForwardManager();

public: //ViperGlobalClass
    virtual void OnViperAllInitialized();

public: //IViperForwardManager
    virtual IViperForward *CreateForward(char const *name, ViperExecType et,
                                         PyObject *types,
                                         IViperForwardCallback callback);
    virtual IViperForward *FindForward(char const *name);
    virtual void ReleaseForward(IViperForward *forward);

public: //IViperPluginsListener
    virtual void OnPluginUnloaded(IViperPlugin *plugin);

protected:
    /**
     * @brief Creates a new CForward or retrieves an old one
     */
    CForward *ForwardMake();
    
    /**
     * @brief Adds the CForward to the free list
     */
    void ForwardFree(CForward *fwd);

private:
    SourceHook::CStack<CForward *> m_FreeForwards;
    SourceHook::List<CForward *> m_ForwardsList;
    Trie *m_Forwards;
};

extern CForwardManager g_Forwards;

#endif /* _INCLUDE_VIPER_FORWARDSYS_H_ */


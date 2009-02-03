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

#include "ForwardSys.h"
#include "viper_globals.h"

//=======================================
// CForward
//=======================================
CForward::~CForward()
{
    if (m_sName != NULL)
        delete [] m_sName;
}

PyObject *
CForward::Execute(int *result, PyObject *args)
{
    PyEval_ReleaseLock();
    
    if (PyTuple_Size(args) != m_iTypesCnt)
        return NULL;
    
    int i;
    for (i=0; i<m_iTypesCnt; i++)
    {
        if (!PyObject_IsInstance(PyTuple_GetItem(args, i),
            PyTuple_GetItem(m_types, i)))
        {
            return NULL;
        }
    }
    
    PyEval_AcquireLock();
    
    PyThreadState *current = PyThreadState_Get();
    
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    IViperPluginFunction *func;
    PyObject *py_result;
    
    unsigned int failed=0, success=0;
    int cur_result;
    int high_result, low_result;
    
    for (iter=m_functions.begin(); iter!=m_functions.end(); iter++)
    {
        func = (*iter);
        py_result = func->Execute(args, NULL);
        
        if (result == NULL)
            failed++;
        else
        {
            success++;
            
            if (m_callback != NULL)
                cur_result = m_callback(py_result, func);
            else if (PyInt_Check(py_result))
            {
                PyEval_ReleaseLock();
                PyThreadState_Swap(func->GetOwnerPlugin()->GetThreadState());
                cur_result = PyInt_AsLong(py_result);
                PyThreadState_Swap(current);
                PyEval_AcquireLock();
            }
            else
                continue;
            
            switch (m_ExecType)
            {
            case ET_Event:
                {
                    if (cur_result > high_result)
                        high_result = cur_result;
                    break;
                }
            case ET_Hook:
                {
                    if (cur_result > high_result)
                    {
                        high_result = cur_result;
                        if ((ViperResultType)high_result == Pl_Stop)
                            goto done;
                    }
                    break;
                }
            case ET_LowEvent:
                {
                    if (cur_result < low_result || success == 1)
                        low_result = cur_result;
                    break;
                }
            default:
                {
                    break;
                }
            }
        }
    }
    
done:
    if (success)
    {
        switch (m_ExecType)
        {
        case ET_Ignore:
            {
                cur_result = 0;
                py_result = NULL;
                break;
            }
        case ET_Event:
        case ET_Hook:
            {
                cur_result = high_result;
                break;
            }
        case ET_LowEvent:
            {
                cur_result = low_result;
                break;
            }
        default:
            {
                break;
            }
        }
    }
    
    if (result != NULL)
        *result = cur_result;
    
    return py_result;
}

bool
CForward::RemoveFunction(IViperPluginFunction *func)
{
    m_functions.remove(func);
    
    return true;
}

bool
CForward::RemoveFunctionsOfPlugin(IViperPlugin *plugin)
{
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    IViperPluginFunction *func;
    
    for (iter=m_functions.begin(); iter!=m_functions.end(); iter++)
    {
        func = (*iter);
        if (func->GetOwnerPlugin() == plugin)
            iter = m_functions.erase(iter);
    }
    
    return true;
}

bool
CForward::AddFunction(IViperPluginFunction *func)
{
    if (m_functions.find(func) != m_functions.end())
        return false;
    
    m_functions.push_back(func);
    
    return true;
}

unsigned int
CForward::GetFunctionCount()
{
    return m_functions.size();
}

char const *
CForward::GetForwardName()
{
    return m_sName;
}

ViperExecType
CForward::GetExecType()
{
    return m_ExecType;
}

PyObject *
CForward::GetParamTypes()
{
    return m_types;
}

CForward *
CForward::CreateForward(char const *name, ViperExecType et, PyObject *types,
                        IForwardCallback callback)
{
    if (!PyTuple_Check(types))
        return NULL;
    
    CForward *fwd = g_Forwards.ForwardMake();
    
    fwd->m_ExecType = et;
    fwd->m_functions.clear();
    fwd->m_paused.clear();
    fwd->m_callback = callback;
    fwd->m_sName = sm_strdup(name);
    
    Py_INCREF(types);
    fwd->m_types = types;
    fwd->m_iTypesCnt = PyTuple_Size(types);
    
    return fwd;
}

//=======================================
// CForwardManager
//=======================================
CForwardManager::~CForwardManager()
{
    SourceHook::CStack<CForward *>::iterator iter;
    for (iter=m_FreeForwards.begin(); iter!=m_FreeForwards.end(); iter++)
    {
        delete (*iter);
    }
    
    m_FreeForwards.popall();
}

void
CForwardManager::OnViperAllInitialized()
{
    g_VPlugins.AddPluginListener(this);
}

void
CForwardManager::ForwardFree(CForward *fwd)
{
    if (fwd == NULL)
        return;
    
    m_FreeForwards.push(fwd);
    m_ForwardsList.remove(fwd);
    
    if (IS_STR_FILLED(fwd->GetForwardName()))
        m_Forwards.remove(fwd->GetForwardName());
}

CForward *
CForwardManager::ForwardMake()
{
    CForward *fwd;
    if (m_FreeForwards.empty())
        fwd = new CForward;
    else
    {
        fwd = m_FreeForwards.front();
        m_FreeForwards.pop();
    }
    
    return fwd;
}

void
CForwardManager::ReleaseForward(IViperForward *forward)
{
    ForwardFree(static_cast<CForward *>(forward));
}

IViperForward *
CForwardManager::CreateForward(char const *name, ViperExecType et,
                               PyObject *types, IForwardCallback callback)
{
    CForward *fwd = CForward::CreateForward(name, et, types, callback);
    if (fwd == NULL)
        return NULL;
    
    m_ForwardsList.push_back(fwd);
    
    if (IS_STR_FILLED(name))
        m_Forwards.insert(name, fwd);
    
    return fwd;
}

IViperForward *
CForwardManager::FindForward(char const *name)
{
    if (!IS_STR_FILLED(name))
        return NULL;
    
    CForward **fwd = m_Forwards.retrieve(name);
    if (fwd == NULL)
        return NULL;
    else
        return *fwd;
}

void
CForwardManager::OnPluginUnloaded(IViperPlugin *plugin)
{
    SourceHook::List<CForward *>::iterator iter;
    for (iter=m_ForwardsList.begin(); iter!=m_ForwardsList.end(); iter++)
    {
        (*iter)->RemoveFunctionsOfPlugin(plugin);
    }
}

CForwardManager g_Forwards;


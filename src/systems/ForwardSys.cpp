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
    for (SourceHook::List<IViperPluginFunction *>::iterator iter=m_functions.begin();
         iter!=m_functions.end(); iter++)
    {
        delete (*iter);
    }
    
    m_functions.clear();
    
    if (m_sName != NULL)
        delete [] m_sName;
}

PyObject *
CForward::Execute(int *result, PyObject *args)
{
    if (PyTuple_Size(args) != m_iTypesCnt)
        return NULL;
    
    int i;
    for (i=0; i<m_iTypesCnt; i++)
    {
        if (!PyObject_IsInstance(PyTuple_GetItem(args, i),
            PyTuple_GetItem(m_types, i)))
        {
            PyErr_Format(PyExc_TypeError, "expected type %s for argument %d, "
                "found %s", PyTuple_GetItem(m_types, i)->ob_type->tp_name,
                i, PyTuple_GetItem(args, i)->ob_type->tp_name);
            return NULL;
        }
    }
    
    PyThreadState *current = PyThreadState_Get();
    
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    IViperPluginFunction *func;
    PyObject *py_result;
    
    size_t failed=0, success=0;
    int cur_result;
    int high_result, low_result;
    
    for (iter=m_functions.begin(); iter!=m_functions.end(); iter++)
    {
        func = (*iter);
        py_result = func->Execute(args, NULL); // Returns new reference
        
        if (py_result == NULL)
        {
            if (PyErr_Occurred())
                PyErr_Print();
            
            failed++;
        }
        else
        {
            success++;
            
            if (m_callback != NULL)
            {
                cur_result = m_callback(this, py_result, func);
                Py_DECREF(py_result);
            }
            else if (PyInt_Check(py_result))
            {
                PyThreadState_Swap(func->GetOwnerPlugin()->GetThreadState());
                
                cur_result = PyInt_AsLong(py_result);
                Py_DECREF(py_result);
                
                PyThreadState_Swap(current);
            }
            else
            {
                Py_DECREF(py_result);
                continue;
            }
            
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
        case ET_Single:
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
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    
    for (iter=m_functions.begin(); iter!=m_functions.end(); iter++)
    {
        if ((*iter)->GetFunction() == func->GetFunction())
        {
            delete (*iter);
            m_functions.erase(iter);
            return true;
        }
    }
    
    return false;
}

bool
CForward::RemoveFunction(unsigned int idx)
{
    if (idx > GetFunctionCount())
        return false;
    
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    for (iter=m_functions.begin(); iter!=m_functions.end() && idx; iter++, idx--)
        m_functions.erase(iter);
    
    return true;
}

void
CForward::RemoveFunctionsOfPlugin(IViperPlugin *plugin)
{
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    IViperPluginFunction *func;
    
    for (iter=m_functions.begin(); iter!=m_functions.end(); iter++)
    {
        if ((*iter)->GetOwnerPlugin() == plugin)
        {
            delete (*iter);
            iter = m_functions.erase(iter);
        }
    }
}

void CForward::Clear()
{
    for (SourceHook::List<IViperPluginFunction *>::iterator iter=m_functions.begin();
         iter!=m_functions.end(); iter++)
    {
        delete (*iter);
    }
    
    m_functions.clear();
}

void
CForward::AddFunction(IViperPluginFunction *func)
{
    assert(func != NULL);
    assert(func->GetFunction() != NULL);
    
    IViperPluginFunction *copy = CPluginFunction::CreatePluginFunction(
        func->GetFunction(), func->GetOwnerPlugin());
    
    m_functions.push_back(copy);
    Py_INCREF(copy->GetFunction());
}

IViperPluginFunction *
CForward::GetFunction(unsigned int idx)
{
    if (idx > GetFunctionCount())
        return NULL;
    
    SourceHook::List<IViperPluginFunction *>::iterator iter;
    for (iter=m_functions.begin(); iter!=m_functions.end() && idx; iter++, idx--)
    {}
    
    return (*iter);
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
                        IViperForwardCallback callback)
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
CForwardManager::CForwardManager()
{
    m_Forwards = sm_trie_create();
}

CForwardManager::~CForwardManager()
{
    sm_trie_destroy(m_Forwards);
    
    SourceHook::CStack<CForward *>::iterator iter;
    for (iter=m_FreeForwards.begin(); iter!=m_FreeForwards.end(); iter++)
    {
        (*iter)->Clear();
        delete (*iter);
    }
    
    m_FreeForwards.popall();
}

void
CForwardManager::OnViperAllInitialized()
{
    g_VPlugins.AddPluginsListener(this);
}

void
CForwardManager::ForwardFree(CForward *fwd)
{
    if (fwd == NULL)
        return;
    
    /* The param types tuple is no longer being used */
    Py_XDECREF(fwd->GetParamTypes());
    
    /* Delete all our IViperPluginFunctions */
    fwd->Clear();
    
    m_FreeForwards.push(fwd);
    m_ForwardsList.remove(fwd);
    
    if (IS_STR_FILLED(fwd->GetForwardName()))
        sm_trie_delete(m_Forwards, fwd->GetForwardName());
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
                               PyObject *types, IViperForwardCallback callback)
{
    CForward *fwd = CForward::CreateForward(name, et, types, callback);
    if (fwd == NULL)
        return NULL;
    
    m_ForwardsList.push_back(fwd);
    
    if (IS_STR_FILLED(name))
        sm_trie_insert(m_Forwards, name, fwd);
    
    return fwd;
}

IViperForward *
CForwardManager::FindForward(char const *name)
{
    if (!IS_STR_FILLED(name))
        return NULL;
    
    CForward *fwd;
    if (!sm_trie_retrieve(m_Forwards, name, (void **)&fwd))
        return NULL;
    else
        return fwd;
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


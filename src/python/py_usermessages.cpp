/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2011 Zach "theY4Kman" Kanzler
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

#include <Python.h>
#include <IForwardSys.h>
#include "IViperForwardSys.h"
#include "PlayerManager.h"
#include <sh_tinyhash.h>
#include "py_bitbuf.h"
#include "py_clients.h"
#include "python_delay_data.h"

using SourceMod::IGamePlayer;
using SourceMod::IUserMessageListener;
using SourceMod::ResultType;

struct UserMsgHookNotify
{
    PyObject *hook;
    PyObject *notify;
};

struct ViperUMListener
{
    ViperUMListener() : intercepts(0), hooks(0), notify(0) {}
    
    /* args: (msg_id, bf, players, reliable, init) */
    IViperForward *intercepts;
    IViperForward *hooks;
    /* args: (msg_id, sent) */
    IViperForward *notify;
};

namespace SourceHook
{
    template<>
    int HashFunction<int>(int const &k)
    {
        return k;
    }

    template<>
    int Compare<int>(int const &k1, int const &k2)
    {
        return (k1-k2);
    }
    
    template<>
    int HashFunction<PyObject *>(PyObject * const &k)
    {
        return reinterpret_cast<int>(k);
    }

    template<>
    int Compare<PyObject *>(PyObject * const &k1, PyObject * const &k2)
    {
        return (k1-k2);
    }
}

//Converts an IRecipientFilter into a Python list containing the client indices
PyObject *
RecipientToPyList(IRecipientFilter *pFilter)
{
    int len = pFilter->GetRecipientCount();
    PyObject *list = PyList_New(len);
    
    /* sawce */
    if (list == NULL)
        return Py_None;
    
    for (int i=0; i<len; i++)
        PyList_SetItem(list, i, g_Players.GetPythonClient(
            pFilter->GetRecipientIndex(i)));
    
    return list;
}

class ViperUserMessageListener : public IUserMessageListener
{
public: // IUserMessageListener
    virtual void OnUserMessage(int msg_id, bf_write *bf,
        IRecipientFilter *pFilter)
    {
        /* Call hook */
        if (m_Hook == NULL)
            return;
        
        PyObject *args = PyTuple_Pack(5, PyInt_FromLong(msg_id),
            /* TODO: bitbuf */ Py_None, RecipientToPyList(pFilter),
            PyBool_FromLong(pFilter->IsReliable()),
            PyBool_FromLong(pFilter->IsInitMessage()));
        
        m_Hook->Execute(args, NULL);
        Py_DECREF(args);
    }
    
    virtual ResultType InterceptUserMessage(int msg_id, bf_write *bf,
        IRecipientFilter *pFilter)
    {
        /* Call intercepts */
        if (m_Intercept == NULL)
            return (SourceMod::ResultType)Pl_Continue;
        
        PyObject *args = PyTuple_Pack(5, PyInt_FromLong(msg_id),
            /* TODO: bitbuf */ Py_None, RecipientToPyList(pFilter),
            PyBool_FromLong(pFilter->IsReliable()),
            PyBool_FromLong(pFilter->IsInitMessage()));
        
        PyObject *py_res = m_Intercept->Execute(args, NULL);
        Py_DECREF(args);
        
        if (PyInt_Check(py_res))
            return (SourceMod::ResultType)PyInt_AS_LONG(py_res);
        
        return (SourceMod::ResultType)Pl_Continue;
    }
    
    virtual void OnPostUserMessage(int msg_id, bool sent)
    {
        /* Call notify */
        if (m_Notify == NULL)
            return;
        
        PyObject *args = PyTuple_Pack(2, PyInt_FromLong(msg_id),
            PyBool_FromLong(sent));
        
        m_Notify->Execute(args, NULL);
        Py_DECREF(args);
    }

public:
    ViperUserMessageListener() : m_Hook(NULL), m_Notify(NULL), m_Intercept(NULL)
    {
    }
    
    void SetHook(IViperPluginFunction *hook)
    {
        m_Hook = hook;
    }
    
    void SetNotify(IViperPluginFunction *notify)
    {
        m_Notify = notify;
    }
    
    void SetIntercept(IViperPluginFunction *intercept)
    {
        m_Intercept = intercept;
    }
    
    bool HasCallback(IViperPluginFunction *func, bool intercept)
    {
        if (!intercept)
        {
            if (m_Hook == NULL)
                return false;
            
            return (m_Hook->GetFunction() == func->GetFunction() &&
                    m_Hook->GetOwnerPlugin() == func->GetOwnerPlugin());
        }
        else
        {
            if (m_Intercept == NULL)
                return false;
            
            return (m_Intercept->GetFunction() == func->GetFunction() &&
                    m_Intercept->GetOwnerPlugin() == func->GetOwnerPlugin());
        }
        
        /* sawce */
        return false;
    }

private:
    IViperPluginFunction *m_Hook;
    IViperPluginFunction *m_Notify;
    IViperPluginFunction *m_Intercept;
};

class ViperUserMessages :
    public ViperGlobalClass,
    public IViperPluginsListener
{
public: // ViperGlobalClass
    virtual void OnViperAllInitialized()
    {
        g_VPlugins.AddPluginsListener(this);
    }

public: // IViperPluginsListener
    virtual void OnPluginUnloaded(IViperPlugin *plugin)
    {
        SourceHook::List<ViperUserMessageListener *> *pList;
        if (!plugin->GetProperty("UserMsgHooks", (void **)&pList, true) ||
            pList == NULL)
            return;
        
        SourceHook::List<ViperUserMessageListener *>::iterator iter;
        for (iter=pList->begin(); iter!=pList->end(); iter++)
        {
            ViperUserMessageListener *listen = (*iter);
            pList->erase(iter);
            delete listen;
        }
    }
};

ViperUserMessages ViperUserMsgs;

static PyObject *
usermessages__begin(PyObject *self)
{
    return GetBitBuf();
}

static PyObject *
usermessages__get_message_index(PyObject *args)
{
    char *msg_name;
    if (!PyArg_ParseTuple(args, "s", &msg_name))
        return NULL;
    
    return PyInt_FromLong(usermsgs->GetMessageIndex(msg_name));
}

static PyObject *
usermessages__get_message_name(PyObject *args)
{
    int msg_id;
    if (!PyArg_ParseTuple(args, "i", &msg_id))
        return NULL;
    
    /* The 64 is arbitrary, but I've never seen a name longer than it */
    char msg_name[64];
    if (!usermsgs->GetMessageName(msg_id, (char *)&msg_name, sizeof(msg_name)))
        Py_RETURN_NONE;
    
    return PyString_FromString((char const *)&msg_name);
}

static PyObject *
usermessages__hook(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *py_usermsg;
    PyObject *callback;
    bool intercept = false;
    PyObject *post_callback = NULL;
    
    int msg_index;
    
    static char *keywdlist[] = {"usermsg", "callback", "intercept",
        "post_callback", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|bO", keywdlist,
            &py_usermsg, &callback, &intercept, &post_callback))
        return NULL;
    
    /* Get the usermessage index from `usermsg` */
    if (PyString_Check(py_usermsg))
    {
        char const *msg_name = PyString_AS_STRING(py_usermsg);
        msg_index = usermsgs->GetMessageIndex(msg_name);
        
        if (msg_index == -1)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " of name \"%s\". Note that usermessage names are"
                " case-sensitive.", msg_name);
            return NULL;
        }
    }
    else if (PyInt_Check(py_usermsg))
    {
        msg_index = PyInt_AS_LONG(py_usermsg);
        
        if (g_SMAPI->GetUserMessage(msg_index) == NULL)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " with ID %d.", msg_index);
            return NULL;
        }
    }
    else
    {
        PyErr_Format(_PyExc_TypeError, "usermsg is the wrong type"
            " (expected str or int, got \"%s\").",
            py_usermsg->ob_type->tp_name);
        return NULL;
    }
    
    /* Check `callback` is callable */
    if (!PyCallable_Check(callback))
    {
        PyErr_SetString(_PyExc_TypeError, "`callback` is not callable.");
        return NULL;
    }
    
    /* Check `post_callback` is callable */
    if ((post_callback != Py_None && post_callback != NULL)
        && !PyCallable_Check(post_callback))
    {
        PyErr_SetString(_PyExc_TypeError, "`post_callback` is not callable.");
        return NULL;
    }
    
    GET_THREAD_PLUGIN();
    
    ViperUserMessageListener *listen = new ViperUserMessageListener();
    
    bool result = usermsgs->HookUserMessage(msg_index, listen, intercept);
    if (!result)
        Py_RETURN_FALSE;
    
    /* Retrieve or create the plug-in's list of listeners */
    SourceHook::List<ViperUserMessageListener *> *pList;
    if (!pPlugin->GetProperty("UserMsgHooks", (void **)&pList, true) ||
        pList == NULL)
    {
        pList = new SourceHook::List<ViperUserMessageListener *>();
        pPlugin->SetProperty("UserMsgHooks", pList);
    }
    
    pList->push_back(listen);
    
    IViperPluginFunction *func = CPluginFunction::CreatePluginFunction(callback,
        pPlugin);
    
    if (!intercept)
        listen->SetHook(func);
    else
        listen->SetIntercept(func);
    
    /* The post_callback must be hooked with HookUserMessage2 */
    if (post_callback != NULL)
    {
        IViperPluginFunction *pFunc = CPluginFunction::CreatePluginFunction(
            post_callback, pPlugin);
        listen->SetNotify(pFunc);
        
        if (!usermsgs->HookUserMessage2(msg_index, listen, intercept))
            Py_RETURN_FALSE;
    }
    
    Py_RETURN_TRUE;
}

static PyObject *
usermessages__send(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *py_usermsg;
    PyObject *bitbuf;
    PyObject *clients;
    int flags = 0;
    
    int msg_index;
    
    static char *keywdlist[] = {"usermsg", "bitbuffer", "clients",
        "flags", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO!O!|i", keywdlist,
        &py_usermsg, &bitbuf__BitBufType, &bitbuf, _PyList_Type,
        &clients, &flags))
        return NULL;
    
    if (PyString_Check(py_usermsg))
    {
        char const *msg_name = PyString_AS_STRING(py_usermsg);
        msg_index = usermsgs->GetMessageIndex(msg_name);
        
        if (msg_index == -1)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " of name \"%s\". Note that usermessage names are"
                " case-sensitive.", msg_name);
            return NULL;
        }
    }
    else if (PyInt_Check(py_usermsg))
    {
        msg_index = PyInt_AS_LONG(py_usermsg);
        
        if (g_SMAPI->GetUserMessage(msg_index) == NULL)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " with ID %d.", msg_index);
            return NULL;
        }
    }
    else
    {
        PyErr_Format(_PyExc_TypeError, "usermsg is the wrong type"
            " (expected str or int, got \"%s\").",
            py_usermsg->ob_type->tp_name);
        return NULL;
    }
    
    // Extract and error-check clients list
    unsigned int numPlayers = PyList_Size(clients);
    
    // Always return true if there are no clients to send to
    if (numPlayers == 0)
        Py_RETURN_TRUE;
    
    cell_t *players = new cell_t[numPlayers];
    for (unsigned int i=0; i<numPlayers; i++)
    {
        PyObject *pyclient = PyList_GetItem(clients, i);
        cell_t client;
        
        if (PyObject_IsInstance(pyclient, (PyObject *)&clients__ClientType))
            client = ((clients__Client*)pyclient)->index;
        else if (PyInt_Check(pyclient))
            client = PyInt_AS_LONG(pyclient);
        else
        {
            PyErr_Format(_PyExc_TypeError, "Object in `clients`\n"
                " at index %d is the wrong type (expected"
                " clients.Client or int, got \"%s\")", i,
                pyclient->ob_type->tp_name);
            return NULL;
        }
        
        IGamePlayer *player = playerhelpers->GetGamePlayer(client);
        if (player == NULL)
        {
            PyErr_Format(g_pViperException, "Client index %d (of `clients`"
                " list position %d) is invalid",
                i, client);
            return NULL;
        }
        else if (!player->IsConnected())
        {
            PyErr_Format(g_pViperException, "Client %d (of `clients`"
                " list position %d) is not connected",
                i, client);
            return NULL;
        }
        
        players[i] = client;
    }
    
    bf_write *bf = usermsgs->StartMessage(msg_index, players, numPlayers, flags);
    if (bf == NULL)
    {
        PyErr_SetString(g_pViperException, "Unable to execute a new usermessage"
            " while in hook.");
        return NULL;
    }
    
    if (!BitBufToBfWrite((bitbuf__BitBuf*)bitbuf, bf))
    {
        PyErr_SetString(g_pViperException, "Unable to transfer data to the"
            " bf_write.");
        return NULL;
    }
    
    return PyBool_FromLong(usermsgs->EndMessage());
}

static PyObject *
usermessages__unhook(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *py_usermsg;
    PyObject *callback;
    bool intercept = false;
    
    int msg_index;
    
    static char *keywdlist[] = {"usermsg", "callback", "intercept", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|b", keywdlist,
            &py_usermsg, &callback, &intercept))
        return NULL;
    
    /* Get the usermessage index from `usermsg` */
    if (PyString_Check(py_usermsg))
    {
        char const *msg_name = PyString_AS_STRING(py_usermsg);
        msg_index = usermsgs->GetMessageIndex(msg_name);
        
        if (msg_index == -1)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " of name \"%s\". Note that usermessage names are"
                " case-sensitive.", msg_name);
            return NULL;
        }
    }
    else if (PyInt_Check(py_usermsg))
    {
        msg_index = PyInt_AS_LONG(py_usermsg);
        
        if (g_SMAPI->GetUserMessage(msg_index) == NULL)
        {
            PyErr_Format(g_pViperException, "Invalid usermessage"
                " with ID %d.", msg_index);
            return NULL;
        }
    }
    else
    {
        PyErr_Format(_PyExc_TypeError, "usermsg is the wrong type"
            " (expected str or int, got \"%s\").",
            py_usermsg->ob_type->tp_name);
        return NULL;
    }
    
    /* Check `callback` is callable */
    if (!PyCallable_Check(callback))
    {
        PyErr_SetString(_PyExc_TypeError, "`callback` is not callable.");
        return NULL;
    }
    
    GET_THREAD_PLUGIN();
    
    SourceHook::List<ViperUserMessageListener *> *pList;
    if (!pPlugin->GetProperty("UserMsgHooks", (void **)&pList, true) ||
        pList == NULL)
        Py_RETURN_NONE;
    
    IViperPluginFunction *func = CPluginFunction::CreatePluginFunction(callback,
        pPlugin);
    
    SourceHook::List<ViperUserMessageListener *>::iterator iter;
    for (iter=pList->begin(); iter!=pList->end(); iter++)
    {
        if ((*iter)->HasCallback(func, intercept))
        {
            ViperUserMessageListener *listen = (*iter);
            pList->erase(iter);
            delete listen;
            
            break;
        }
    }
    
    Py_RETURN_NONE;
}

static PyMethodDef usermessages__methods[] = {
    {"begin", (PyCFunction)usermessages__begin, METH_NOARGS,
        "begin() -> bitbuf.BitBuf\n\n"
        "Returns a bitbuf.BitBuf object to be used in sending a usermessage.\n"
        "@rtype: sourcemod.bitbuf.BitBuf\n"
        "@return: A writable bitbuf.BitBuf object."},
    {"get_message_index", (PyCFunction)usermessages__get_message_index, METH_VARARGS,
        "get_message_index(msg_name) -> int\n\n"
        "Get the user message index of the user message `msg_name`\n"
        "@type  msg_name: str\n"
        "@param msg_name: The name of the usermessage to find the index for.\n"
        "@rtype: int\n"
        "@return: The index of the usermessage on success, -1 otherwise."},
    {"get_message_name", (PyCFunction)usermessages__get_message_name, METH_VARARGS,
        "get_message_name(msg_id) -> str\n\n"
        "Retrieves the name of a usermessage by its index/ID.\n"
        "@type  msg_id: int\n"
        "@param msg_id: The index of the usermessage to get the name of.\n"
        "@rtype: str\n"
        "@return: The name of the usermessage on success, None otherwise."},
    {"hook", (PyCFunction)usermessages__hook, METH_VARARGS|METH_KEYWORDS,
        "hook(usermsg, callback[, intercept=False[, post_callback=None]]) -> bool\n\n"
        "Hooks a usermessage\n"
        "@type  usermsg: str or int\n"
        "@param usermsg: The name or ID of the usermessage to hook.\n"
        "@type  callback: callable\n"
        "@param callback: The function to use as a hook.\n"
        "@type  intercept: bool\n"
        "@param intercept: If True, message will be fully intercepted, allowing the user\n"
        "    to block the message. Otherwise, the hook is normal and its return value is\n"
        "    ignored.\n"
        "@type  post_callback: callable\n"
        "@param post_callback: Optional notification function.\n"
        "@rtype: bool\n"
        "@return: True if successfully hooked, False otherwise."},
    {"send", (PyCFunction)usermessages__send, METH_VARARGS|METH_KEYWORDS,
        "send(usermsg, bitbuffer, clients[, flags=0]) -> bool\n\n"
        "Sends the usermessage with an ID or name `usermsg` using `bitbuffer`\n"
        "as data to all the clients in `clients`, optionally using `flags`.\n"
        "@type  usermsg: str or int\n"
        "@param usermsg: A string containing the name of the usermessage, or\n"
        "    an integer containing the usermessage's index.\n"
        "@type  bitbuffer: sourcemod.bitbuf.BitBuf\n"
        "@param bitbuffer: A bitbuf.BitBuf object that contains all the data\n"
        "    needed to send the usermessage. This object can be obtained\n"
        "    with the usermessages.begin() function.\n"
        "@type  clients: list\n"
        "@param clients: A list of clients to send the usermessage to. The\n"
        "    list can contain clients.Client objects or integers containing\n"
        "    indices of clients.\n"
        "@type  flags: USERMSG constants\n"
        "@param flags: Optional flags to set. See usermessages.USERMSG_*"},
    {"unhook", (PyCFunction)usermessages__unhook, METH_VARARGS|METH_KEYWORDS,
        "unhook(usermsg, callback[, intercept=False])\n\n"
        "Removes one usermessage hook, and its notification callback, if one was set.\n"
        "@type  usermsg: str or int\n"
        "@param usermsg: A string containing the name of the usermessage, or\n"
        "    an integer containing the usermessage's index.\n"
        "@type  callback: callable\n"
        "@param callback: The function used for the hook.\n"
        "@type  intercept: bool\n"
        "@param intercept: Whether the hook was registered as an intercept hook or not."},
    {NULL, NULL, 0, NULL}
};

PyObject *
initusermessages(void)
{
    PyObject *usermessages = Py_InitModule3("usermessages",
        usermessages__methods, "Module for manipulating usermessages.");
    
    PyModule_AddIntMacro(usermessages, USERMSG_RELIABLE);
    PyModule_AddIntMacro(usermessages, USERMSG_INITMSG);
    PyModule_AddIntMacro(usermessages, USERMSG_BLOCKHOOKS);
    
    return usermessages;
}


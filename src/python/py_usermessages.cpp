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

#include <Python.h>
#include "py_bitbuf.h"
#include "py_clients.h"
#include "python_delay_data.h"

using SourceMod::IGamePlayer;

PyObject *
usermessages__begin(PyObject *self)
{
    return GetBitBuf();
}

PyObject *
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

static PyMethodDef usermessages__methods[] = {
#if NOT_DOCUMENTED_YET
    {"get_message_index"},
    {"hook_usermessage"},
    {"unhook_usermessage"},
#endif
    {"begin", (PyCFunction)usermessages__begin, METH_NOARGS,
        "begin() -> bitbuf.BitBuf\n\n"
        "Returns a bitbuf.BitBuf object to be used in sending a usermessage.\n"
        "@rtype: sourcemod.bitbuf.BitBuf\n"
        "@return: A writable bitbuf.BitBuf object."},
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


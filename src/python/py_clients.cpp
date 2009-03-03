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

/* TODO: Client object cache.
 * NOTE: IGamePlayers are invalidated when max clients changes.
 */

#include <Python.h>
#include <structmember.h>
#include "extension.h"
#include "py_clients.h"
#include <IPlayerHelpers.h>

using SourceMod::IGamePlayer;

#define CHECK_CLIENT_VALID(index, minindex) {\
    CHECK_CLIENT_INRANGE(index, minindex);\
    CHECK_CLIENT_CONNECTED(index);\
}

#define CHECK_CLIENT_INRANGE(index, minindex) {\
    if (index < minindex || index > playerhelpers->GetMaxClients()) \
    { \
        PyErr_Format(g_pViperException, "Client %d is invalid", index); \
        return NULL; \
    } \
}

#define CHECK_CLIENT_CONNECTED(client) {\
    if (!client->IsConnected()) \
    { \
        PyErr_Format(g_pViperException, "Client %d is not connected", index); \
        return NULL; \
    } \
}

static int
clients__Client__init__(clients__Client *self, PyObject *args, PyObject *kwds)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return -1;
    
    if (index < 0 || playerhelpers->GetMaxClients())
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", index);
        return -1;
    }
    
    self->index = index;
    
    return 0;
}

static PyObject *
clients__Client__str__(clients__Client *self)
{
    return PyString_FromFormat("<Client %d>", self->index);
}

static PyObject *
clients__Client__ipget(clients__Client *self)
{
    if (self->index < 1 || self->index > playerhelpers->GetMaxClients())
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *client = playerhelpers->GetGamePlayer(self->index);
    if (!client->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    return PyString_FromString(client->GetIPAddress());
}

static PyMemberDef clients__Client__members[] = {
    {"index", T_INT, offsetof(clients__Client, index), READONLY,
        "The client index of the client"},
    {NULL},
};

static PyGetSetDef clients__Client__getsets[] = {
    {"ip", (getter)clients__Client__ipget, NULL,
        "The IP address of this client.", NULL},
#if NOT_IMPLEMENTED_YET
    {"name", (getter)clients__Client__nameget, (setter)clients__Client__nameset,
        "The name of this client.", NULL},
    {"steamid", (getter)clients__Client__steamidget, NULL,
        "The Steam ID of this client.", NULL},
    {"userid", (getter)clients__Client__useridget, NULL,
        "The userid of this client.", NULL},
#endif
    {NULL},
};

static PyMethodDef clients__Client__methods[] = {
#ifdef NOT_IMPLEMENTED_YET
#endif
    {NULL, NULL, 0, NULL}
};

PyTypeObject clients__ClientType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                          /*ob_size*/
    "sourcemod.clients.Client", /*tp_name*/
    sizeof(clients__Client),    /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    0,                          /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    (reprfunc)clients__Client__str__,/*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Contains methods and members to manipulate a player.",
    0,		                    /* tp_traverse */
    0,		                    /* tp_clear */
    0,		                    /* tp_richcompare */
    0,		                    /* tp_weaklistoffset */
    0,		                    /* tp_iter */
    0,		                    /* tp_iternext */
    clients__Client__methods,   /* tp_methods */
    clients__Client__members,   /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    0,                          /* tp_init */
    0,                          /* tp_alloc */
    PyType_GenericNew,          /* tp_new */
};

static PyMethodDef clients__methods[] = {
#ifdef NOT_IMPLEMENTED_YET
    {"create_fake_client", clients__create_fake_client, METH_VARARGS,
        "create_fake_client(name) -> Client object\n\n"
        "Creates a fake client.\n"
        "@type  name: str\n"
        "@param name: The name to use for the fake client\n"
        "@rtype: sourcemod.clients.Client\n"
        "@return: A valid Client object on success, None otherwise."},
    {"get_client_count", clients__get_client_count, METH_VARARGS,
        "get_client_count([in_game_only=True]) -> int\n\n"
        "Returns the number of clients put in the server\n"
        "@type  in_game_only: bool\n"
        "@param in_game_only: If False, players currently connecting are also counted."},
    {"get_client_of_userid", clients__get_client_of_userid, METH_VARARGS,
        "get_client_of_userid(userid) -> Client object\n"
        "Translates a userid index to a Client object\n"
        "@type  userid: int\n"
        "@param userid: Userid value\n"
        "@rtype: sourcemod.clients.Client\n"
        "@return: A valid Client object on success, None if an invalid userid."},
    {"get_max_clients", (PyCFunction)clients__get_max_clients, METH_NOARGS,
        "get_max_clients() -> int\n\n"
        "Returns the maximum number of clients allowed on the server. This may return 0\n"
        "if called before on_map_start."},
#endif
    {NULL, NULL, 0, NULL},
};

PyObject *
initclients(void)
{
    if (PyType_Ready(&clients__ClientType) < 0)
        return NULL;
    
    PyObject *clients = Py_InitModule3("clients", clients__methods,
        "Contains objects and methods to manipulate and handle clients.");
    
    Py_INCREF((PyObject*)&clients__ClientType);
    PyModule_AddObject(clients, "Client", (PyObject*)&clients__ClientType);
    
    return clients;
}


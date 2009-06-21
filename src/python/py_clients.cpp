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

#include <Python.h>
#include <structmember.h>
#include "extension.h"
#include "py_clients.h"
#include <iplayerinfo.h>
#include <IPlayerHelpers.h>
#include "PlayerManager.h"

using SourceMod::IGamePlayer;

static PyObject *
clients__Client__print_center(clients__Client *self, PyObject *args)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    else if (!player->IsInGame())
    {
        PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
        return NULL;
    }
    
    char *message;
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;
    
    gamehelpers->TextMsg(self->index, HUD_PRINTCENTER, message);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__print_chat(clients__Client *self, PyObject *args)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    else if (!player->IsInGame())
    {
        PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
        return NULL;
    }
    
    char *message;
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;
    
    gamehelpers->TextMsg(self->index, HUD_PRINTTALK, message);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__fakeget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    if (player->IsFakeClient())
        Py_RETURN_TRUE;
    
    Py_RETURN_FALSE;
}

static PyObject *
clients__Client__ipget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    return PyString_FromString(player->GetIPAddress());
}

static PyObject *
clients__Client__lang_idget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    return PyInt_FromLong(player->GetLanguageId());
}

static PyObject *
clients__Client__nameget(clients__Client *self)
{
    if (self->index == 0)
    {
        static ConVar *hostname = NULL;
        if (hostname == NULL)
        {
            hostname = icvar->FindVar("hostname");
            if (hostname == NULL)
            {
                /* Damn it, sawce! It's all your fault hostname is gone! */
                PyErr_SetString(g_pViperException, "Could not find \"hostname\""
                    " cvar");
                return NULL;
            }
        }
        
        return PyString_FromString(hostname->GetString());
    }
    
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    return PyString_FromString(player->GetName());
}

static PyObject *
clients__Client__steamidget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    /* The player has not yet been authorized */
    if (player->GetAuthString() == NULL)
        Py_RETURN_NONE;
    
    return PyString_FromString(player->GetAuthString());
}

static PyObject *
clients__Client__teamget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    else if (!player->IsInGame())
    {
        PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
        return NULL;
    }

	IPlayerInfo *pInfo = player->GetPlayerInfo();
	if (pInfo == NULL)
	{
        PyErr_SetString(g_pViperException, "IPlayerInfo not supported by this game");
        return NULL;
	}

	return PyInt_FromLong(pInfo->GetTeamIndex());
}

static PyObject *
clients__Client__useridget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return NULL;
    }
    
    return PyInt_FromLong(player->GetUserId());
}

static PyObject *
clients__Client__str__(clients__Client *self)
{
    if (self->index > 0)
    {
        IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
        if (player->IsConnected())
        {
            return PyString_FromFormat("<Client %d (%d)::%s::%s::%s>",
                self->index, player->GetUserId(), player->GetName(),
                player->GetAuthString(), player->GetIPAddress());
        }
    }
    
    /* Grab the client's `name` property */
    PyObject *name = clients__Client__nameget(self);
    if (name != NULL)
    {
        return PyString_FromFormat("<Client %d::%s>", self->index,
            PyString_AsString(name));
    }
    else if (PyErr_Occurred())
    {
        /* The error created is like sawce: we don't care */
        PyErr_Clear();
    }
    
    return PyString_FromFormat("<Client %d>", self->index);
}

static PyMemberDef clients__Client__members[] = {
    {"index", T_INT, offsetof(clients__Client, index), READONLY,
        "The client index of the client"},
    {NULL},
};

static PyGetSetDef clients__Client__getsets[] = {
    {"fake", (getter)clients__Client__fakeget, NULL,
        "Whether the client is fake or not.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"ip", (getter)clients__Client__ipget, NULL,
        "The IP address of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"lang_id", (getter)clients__Client__lang_idget, NULL,
        "This client's language id.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"name", (getter)clients__Client__nameget, NULL,
        "The name of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"steamid", (getter)clients__Client__steamidget, NULL,
        "The Steam ID of this client. This is None when the client is not\n"
        "authorized yet.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"team", (getter)clients__Client__teamget, NULL,
        "This player's team.\n"
        "@throw ViperError: Invalid client, client not connected, or client not in-game."},
    {"userid", (getter)clients__Client__useridget, NULL,
        "The userid of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {NULL},
};

static PyMethodDef clients__Client__methods[] = {
#ifdef NOT_IMPLEMENTED_YET
    {"ban", (PyCFunction)clients__Client__ban, METH_VARARGS | METH_KEYWDS,
        "ban(time, flags, reason, kickmsg=\"Kicked\"[, cmd=None[, source=0]]]) -> bool\n\n"
        "Bans the client.\n",
        "@type  time: int\n"
        "@param time: Time, in minutes, to ban (0 = permanent)\n"
        "@type  flags: banning.BANFLAG\n"
        "@param flags: Flags for controlling the ban mechanism. If BANFLAG_AUTHID is\n"
        "    set and no AUTHID is available, the ban will fail unless AUTO is also\n"
        "    flagged.\n"
        "@type  reason: str\n"
        "@param reason: Reason to ban the client for.\n"
        "@type  kickmsg: str\n"
        "@param kickmsg: Message o display to the user when they're kicked.\n"
        "@type  cmd: str\n"
        "@param cmd: Command string to identify the source. If this is left empty,\n"
        "    then the ban_client forward will not be called.\n"
        "@type  source: object\n"
        "@param source: A source value that could be interpreted as the identity of the\n"
        "    player whom was the source of the banning (not actually checked by Core).\n"
        "@rtype: bool\n"
        "@return: True on success, False on failure."},
    {"notify_post_admin_check", (PyCFunction)clients__Client__notify_post_admin_check, METH_VARARGS,
        "notify_post_admin_check()\n\n"
        "Signals that a player has completed post-connection admin checks. Has no effect\n"
        "if the player has already had this event signalled. Note: This must be sent even\n"
        "if no admin id was assigned."},
    {"print_console", (PyCFunction)clients__Client__print_console, METH_VARARGS,
        "print_console(message)\n\n"
        "Prints a message to this client's console.\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"print_hint", (PyCFunction)clients__Client__print_hint, METH_VARARGS,
        "print_hint(message)\n\n"
        "Prints a message to this client with a hint box.\n"
        "@type  message: str\n"
        "@param message: The message to print"}
#endif
    {"print_center", (PyCFunction)clients__Client__print_center, METH_VARARGS,
        "print_center(message)\n\n"
        "Prints a message to this client in the center of the screen.\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"print_chat", (PyCFunction)clients__Client__print_chat, METH_VARARGS,
        "print_chat(message)\n\n"
        "Prints a message to this client's chat area.\n"
        "@type  message: str\n"
        "@param message: The message to print"},
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
    clients__Client__getsets,   /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    0,                          /* tp_init */
    0,                          /* tp_alloc */
    0,                          /* tp_new */
};

static PyObject *
clients__create_fake_client(PyObject *self, PyObject *args)
{
    char *name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    
    edict_t *pEdict = engine->CreateFakeClient(name);
    
    if (pEdict == NULL)
        Py_RETURN_NONE;
    
    PyObject *client = g_Players.GetPythonClient(IndexOfEdict(pEdict));
    Py_XINCREF(client);
    
    return client;
}

static PyObject *
clients__get_client(PyObject *self, PyObject *args)
{
    int index;
    if (!PyArg_ParseTuple(args, "i", &index))
        return NULL;
    
    if (index < 0 || index > playerhelpers->GetMaxClients())
    {
        PyErr_Format(g_pViperException, "client index %d is invalid", index);
        return NULL;
    }
    
    PyObject *client = g_Players.GetPythonClient(index);
    Py_XINCREF(client);
    
    return client;
}

static PyObject *
clients__get_client_count(PyObject *self, PyObject *args)
{
    bool in_game_only = true;
    if (!PyArg_ParseTuple(args, "|b", &in_game_only))
        return NULL;
    
    if (in_game_only)
        return PyInt_FromLong(playerhelpers->GetNumPlayers());
    
    int maxplayers = playerhelpers->GetMaxClients();
    int count = 0;
    for (int i=1; i<=maxplayers; ++i)
    {
        IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(i);
        if ((pPlayer->IsConnected()) && !(pPlayer->IsInGame()))
            count++;
    }
    
    return PyInt_FromLong(playerhelpers->GetNumPlayers() + count);
}

static PyObject *
clients__get_client_of_userid(PyObject *self, PyObject *args)
{
    int userid;
    if (!PyArg_ParseTuple(args, "i", &userid))
        return NULL;
    
    if (userid < 0)
    {
        PyErr_Format(g_pViperException, "Userid %d is invalid", userid);
        return NULL;
    }
    
    int index = playerhelpers->GetClientOfUserId(userid);
    if (index == 0)
        Py_RETURN_NONE;
    
    PyObject *client = g_Players.GetPythonClient(index);
    Py_XINCREF(client);
    
    return client;
}

static PyObject *
clients__get_max_clients(PyObject *self, PyObject *args)
{
    return PyInt_FromLong(playerhelpers->GetMaxClients());
}

static PyMethodDef clients__methods[] = {
    {"create_fake_client", clients__create_fake_client, METH_VARARGS,
        "create_fake_client(name) -> Client object\n\n"
        "Creates a fake client.\n"
        "@type  name: str\n"
        "@param name: The name to use for the fake client\n"
        "@rtype: sourcemod.clients.Client\n"
        "@return: A valid Client object on success, None otherwise."},
    {"get_client", clients__get_client, METH_VARARGS,
        "get_client(index) -> Client object\n\n"
        "Retrieves the Client object of that client index\n"
        "@type  index: int\n"
        "@param index: The client index to find the Client object for\n"
        "@rtype: sourcemod.clients.Client\n"
        "@return: A valid Client object on success, None if an invalid client index."},
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


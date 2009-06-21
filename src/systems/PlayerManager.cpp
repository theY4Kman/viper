/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software
{
}
 you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY
{
}
 without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file    PlayerManager.cpp
 * @brief   Registers and creates forwards for the SM client listener
 */

#include "PlayerManager.h"
#include "python/py_clients.h"

/** Used for the OnClientConnect forward */
char *g_RejectMsg = NULL;
int g_RejectMsgLen = 0;

ViperPlayerManager::ViperPlayerManager()
{
}

void
ViperPlayerManager::OnViperStartup(bool late)
{
    m_Clients = new PyObject*[ABSOLUTE_PLAYER_LIMIT + 1];
    memset(m_Clients, 0, sizeof(PyObject*) * (ABSOLUTE_PLAYER_LIMIT + 1));
    
    Py_INCREF(&clients__ClientType);
    PyObject *pySingleClientArgs = PyTuple_Pack(1, &clients__ClientType);
    
    m_OnClientConnect = g_Forwards.CreateForward("client_connect",
        ET_Hook, pySingleClientArgs, InterceptClientConnectCallback);
    m_OnClientConnected = g_Forwards.CreateForward("client_connected",
        ET_Ignore, pySingleClientArgs, NULL);
    m_OnClientPutInServer = g_Forwards.CreateForward("client_put_in_server",
        ET_Ignore, pySingleClientArgs, NULL);
    m_OnClientDisconnecting = g_Forwards.CreateForward("client_disconnecting",
        ET_Ignore, pySingleClientArgs, NULL);
    m_OnClientDisconnected = g_Forwards.CreateForward("client_disconnected",
        ET_Ignore, pySingleClientArgs, NULL);
    m_OnClientAuthorized = g_Forwards.CreateForward("client_authorized",
        ET_Ignore, PyTuple_Pack(2, &clients__ClientType, &PyString_Type), NULL);
    
    m_OnClientPreAdminCheck = g_Forwards.CreateForward("client_pre_admin_check",
        ET_Hook, pySingleClientArgs, NULL);
    m_OnClientPostAdminCheck = g_Forwards.CreateForward("client_post_admin_check",
        ET_Ignore, pySingleClientArgs, NULL);
    
    m_OnServerActivate = g_Forwards.CreateForward("server_activate", ET_Ignore,
        PyTuple_Pack(1, &PyInt_Type), NULL);
    m_OnMapStart = g_Forwards.CreateForward("map_start", ET_Ignore,
        pySingleClientArgs, NULL);
    
    playerhelpers->AddClientListener(this);
}

void
ViperPlayerManager::OnViperShutdown()
{
    g_Forwards.ReleaseForward(m_OnClientConnect);
    g_Forwards.ReleaseForward(m_OnClientConnected);
    g_Forwards.ReleaseForward(m_OnClientPutInServer);
    g_Forwards.ReleaseForward(m_OnClientDisconnecting);
    g_Forwards.ReleaseForward(m_OnClientDisconnected);
    g_Forwards.ReleaseForward(m_OnClientAuthorized);
    
    g_Forwards.ReleaseForward(m_OnClientPreAdminCheck);
    g_Forwards.ReleaseForward(m_OnClientPostAdminCheck);
    
    g_Forwards.ReleaseForward(m_OnServerActivate);
    g_Forwards.ReleaseForward(m_OnMapStart);
    
    playerhelpers->RemoveClientListener(this);
    
    for (unsigned int i=0; i<sizeof(m_Clients); i++)
        Py_XDECREF(m_Clients[i]);
}

bool
ViperPlayerManager::InterceptClientConnect(int client, char *reject, int maxrejectlen)
{
    g_RejectMsg = reject;
    g_RejectMsgLen = maxrejectlen;
    
    int result;
    m_OnClientConnect->Execute(&result, PyTuple_Pack(1, client));
    
    /* result comes directly from InterceptClientConnectCallback */
    return !((bool)result);
}

ViperResultType InterceptClientConnectCallback(PyObject *ret,
                                               IViperPluginFunction *func)
{
    /* The plug-in wants to let the player in, continue */
    if (ret == Py_None || ret == Py_True)
        return Pl_Continue;
    
    /* The plug-in wants to reject the player, their return object will be used
     * as a reject message.
     */
    PyObject *py_str = PyObject_Str(ret);
    PyString_AsStringAndSize(py_str, &g_RejectMsg, &g_RejectMsgLen);
    Py_DECREF(py_str);
    
    return Pl_Stop;
}

void
ViperPlayerManager::OnClientConnected(int client)
{
    m_OnClientConnected->Execute(NULL, PyTuple_Pack(1, GetPythonClient(client)));
}

void
ViperPlayerManager::OnClientPutInServer(int client)
{
    m_OnClientPutInServer->Execute(NULL, PyTuple_Pack(1, GetPythonClient(client)));
}

void
ViperPlayerManager::OnClientDisconnecting(int client)
{
    m_OnClientDisconnecting->Execute(NULL, PyTuple_Pack(1, GetPythonClient(client)));
}

void
ViperPlayerManager::OnClientDisconnected(int client)
{
    m_OnClientDisconnected->Execute(NULL, PyTuple_Pack(1, GetPythonClient(client)));
}

void
ViperPlayerManager::OnClientAuthorized(int client, char const *authstring)
{
    m_OnClientAuthorized->Execute(NULL,
        PyTuple_Pack(2, GetPythonClient(client), PyString_FromString(authstring)));
}

bool
ViperPlayerManager::OnClientPreAdminCheck(int client)
{
    int result;
    m_OnClientPreAdminCheck->Execute(&result, PyTuple_Pack(1, GetPythonClient(client)));
    
    return (bool)result;
}

void
ViperPlayerManager::OnClientPostAdminCheck(int client)
{
    m_OnClientPostAdminCheck->Execute(NULL, PyTuple_Pack(1, GetPythonClient(client)));
}

void
ViperPlayerManager::OnServerActivate(int clientMax)
{
    m_OnServerActivate->Execute(NULL, PyTuple_Pack(1, PyInt_FromLong(clientMax)));
}

PyObject *
ViperPlayerManager::GetPythonClient(int client)
{
    if (client < 0 || client > playerhelpers->GetMaxClients())
        return NULL;
    
    if (m_Clients[client] != NULL)
        return m_Clients[client];
    
    clients__Client *newclient = PyObject_GC_New(clients__Client,
        &clients__ClientType);
    newclient->index = client;
    
    Py_INCREF((PyObject*)newclient);
    m_Clients[client] = (PyObject*)newclient;
    
    return m_Clients[client];
}

ViperPlayerManager g_Players;


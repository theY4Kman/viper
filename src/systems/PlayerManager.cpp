/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
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

#include <IPlayerHelpers.h>
#include "PlayerManager.h"
#include "ConCmdManager.h"
#include "python/py_clients.h"
#include "viper.h"

#if SOURCE_ENGINE >= SE_ORANGEBOX
SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *, const CCommand &);
#else
SH_DECL_HOOK1_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *);
#endif

IViperForward *g_pViperOnBanClient;

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
    
    PyObject *pySingleClientArgs = PyTuple_Pack(1, &clients__ClientType);
    PyObject *pyClientStringArgs = PyTuple_Pack(2, &clients__ClientType, _PyString_Type);
    PyObject *pySingleIntArgs = PyTuple_Pack(1, _PyInt_Type);
    
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
        ET_Ignore, pyClientStringArgs, NULL);
    
    m_OnClientPreAdminCheck = g_Forwards.CreateForward("client_pre_admin_check",
        ET_Hook, pySingleClientArgs, NULL);
    m_OnClientPostAdminCheck = g_Forwards.CreateForward("client_post_admin_check",
        ET_Ignore, pySingleClientArgs, NULL);
    
    m_OnServerActivate = g_Forwards.CreateForward("server_activate", ET_Ignore,
        pySingleIntArgs, NULL);
    m_OnMapStart = g_Forwards.CreateForward("map_start", ET_Ignore,
        pySingleClientArgs, NULL);
    
    Py_DECREF(pySingleClientArgs);
    Py_DECREF(pyClientStringArgs);
    Py_DECREF(pySingleIntArgs);
    
    PyObject *pyBanClientArgs = PyTuple_Pack(7, &clients__ClientType,
        _PyInt_Type, _PyInt_Type, _PyString_Type, _PyString_Type,
        _PyString_Type, _PyInt_Type);
    
    g_pViperOnBanClient = g_Forwards.CreateForward("ban_client", ET_Event,
        pyBanClientArgs, NULL);
    
    Py_DECREF(pyBanClientArgs);
    
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, serverClients,
        this, &ViperPlayerManager::OnClientCommand, false);
    
    playerhelpers->AddClientListener(this);
}

void
ViperPlayerManager::OnViperShutdown()
{
    playerhelpers->RemoveClientListener(this);
    
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
    
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, serverClients,
        this, &ViperPlayerManager::OnClientCommand, false);
    
    for (unsigned int i=0; i<sizeof(m_Clients); i++)
        Py_XDECREF(m_Clients[i]);
}

bool
ViperPlayerManager::InterceptClientConnect(int client, char *reject, size_t maxrejectlen)
{
    if (m_OnClientConnect->GetFunctionCount() == 0)
        return true;
    
    g_RejectMsg = reject;
    g_RejectMsgLen = maxrejectlen;
    
    int result;
    PyObject *args = PyTuple_Pack(1, GetPythonClient(client));
    m_OnClientConnect->Execute(&result, args);
    Py_DECREF(args);
    
    /* result comes directly from InterceptClientConnectCallback */
    return !result;
}

void
#if SOURCE_ENGINE >= SE_ORANGEBOX
ViperPlayerManager::OnClientCommand(edict_t *pEntity, const CCommand &args)
{
#else
ViperPlayerManager::OnClientCommand(edict_t *pEntity)
{
    CCommand args;
#endif
    int client = IndexOfEdict(pEntity);
    SourceMod::IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(client);
    
    if (!pPlayer->IsConnected())
        return;
    
    g_Viper.PushCommandStack(&args);
    
    int argcount = args.ArgC() - 1;
    char const *cmd = g_Viper.CurrentCommandName();
    
    ViperResultType res = Pl_Continue;
    
    /* TODO: OnClientCommand forward */
    
    res = g_VCmds.DispatchClientCommand(client, cmd, argcount, res);
    
    g_Viper.PopCommandStack();
    
    if (res >= Pl_Handled)
        RETURN_META(MRES_SUPERCEDE);
}

ViperResultType
InterceptClientConnectCallback(IViperForward *fwd, PyObject *ret,
                               IViperPluginFunction *func)
{
    /* Now accepts a return value of (bool let_user_in, str reject_msg) */
    /* The plug-in wants to let the player in, continue */
    if (ret == Py_None || ret == Py_True)
        return Pl_Continue;
    
    /* The plug-in wants to kick the user with the default kick message */
    if (ret == Py_False)
        return Pl_Stop;
    
    if (!PyTuple_Check(ret))
    {
        PyErr_WarnEx(_PyExc_RuntimeWarning, "on_client_connect callbacks should "
            "only return either a boolean or a tuple(bool, str).", 1);
        return Pl_Continue;
    }
    
    if (PyTuple_GET_SIZE(ret) != 2)
    {
        PyErr_WarnEx(_PyExc_RuntimeWarning, "on_client_connect callbacks should "
            "only return either a boolean or a tuple(bool, str).", 1);
        return Pl_Continue;
    }
    
    PyObject *let_user_in = PyTuple_GET_ITEM(ret, 0);
    PyObject *reject_msg = PyTuple_GET_ITEM(ret, 1);
    
    /* The plug-in wants to reject the player, their return object will be used
     * as a reject message.
     */
    PyObject *py_str = PyObject_Str(reject_msg);
    PyString_AsStringAndSize(py_str, &g_RejectMsg, &g_RejectMsgLen);
    Py_DECREF(py_str);
    
    if (let_user_in != Py_False && let_user_in != Py_True)
    {
        PyErr_WarnEx(_PyExc_RuntimeWarning, "on_client_connect callbacks should "
            "only return either a boolean or a tuple(bool, str).", 1);
        return Pl_Continue;
    }
    
    return (let_user_in == Py_True) ? Pl_Continue : Pl_Stop;
}

#define PY_CLIENT_FORWARD(forward) \
    void \
    ViperPlayerManager::forward(int client) \
    { \
        PyObject *args = PyTuple_Pack(1, GetPythonClient(client)); \
        m_##forward->Execute(NULL, args); \
        Py_DECREF(args); \
    }

PY_CLIENT_FORWARD(OnClientConnected);
PY_CLIENT_FORWARD(OnClientPutInServer);
PY_CLIENT_FORWARD(OnClientDisconnecting);
PY_CLIENT_FORWARD(OnClientDisconnected);
PY_CLIENT_FORWARD(OnClientPostAdminCheck);

void
ViperPlayerManager::OnClientAuthorized(int client, char const *authstring)
{
    PyObject *args = PyTuple_Pack(2, GetPythonClient(client),
        PyString_FromString(authstring));
    m_OnClientAuthorized->Execute(NULL, args);
    Py_DECREF(args);
}

bool
ViperPlayerManager::OnClientPreAdminCheck(int client)
{
    int result;
    PyObject *args = PyTuple_Pack(1, GetPythonClient(client));
    m_OnClientPreAdminCheck->Execute(&result, args);
    Py_DECREF(args);
    
    return !!result;
}
void
ViperPlayerManager::OnServerActivate(int clientMax)
{
    PyObject *args = PyTuple_Pack(1, PyInt_FromLong(clientMax));
    m_OnServerActivate->Execute(NULL, args);
    Py_DECREF(args);
}

PyObject *
ViperPlayerManager::GetPythonClient(int client)
{
    if (client < 0 || client > playerhelpers->GetMaxClients())
        return NULL;
    
    if (m_Clients[client] != NULL)
    {
        Py_INCREF(m_Clients[client]);
        return m_Clients[client];
    }
    
    clients__Client *newclient = PyObject_New(clients__Client,
        &clients__ClientType);
    newclient->index = client;
    
    Py_INCREF((PyObject*)newclient);
    m_Clients[client] = (PyObject*)newclient;
    
    return m_Clients[client];
}

ViperPlayerManager g_Players;


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

#include <Python.h>
#include <structmember.h>
#include "extension.h"
#include "py_clients.h"
#include "py_entity.h"
#include "py_keyvalues.h"
#include <iplayerinfo.h>
#include <inetchannel.h>
#include <IPlayerHelpers.h>
#include "HalfLife2.h"
#include "PlayerManager.h"

/* TODO: Move these to sourcemod.banning module */
#define BANFLAG_AUTO	(1<<0)	/**< Auto-detects whether to ban by steamid or IP */
#define BANFLAG_IP   	(1<<1)	/**< Always ban by IP address */
#define BANFLAG_AUTHID	(1<<2)	/**< Ban by SteamID */
#define BANFLAG_NOKICK	(1<<3)	/**< Does not kick the client */
#define BANFLAG_NOWRITE	(1<<4)	/**< Ban is not written to SourceDS's files if permanent */
#define BANTIME_FOREVER 0

using SourceMod::IGamePlayer;

static PyObject *
clients__Client__ban(clients__Client *self, PyObject *args, PyObject *kwds)
{
    int ban_time;
    int ban_flags;
    char const *ban_reason;
    
    char const *ban_kickmsg = "Kicked";
    char const *ban_cmd = "";
    int ban_source = 0;
    
    static char *keywdlist[] = {"time", "flags", "reason", "kickmsg", "cmd",
                                "source"};
    
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
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iis|ssi", keywdlist,
        &ban_time, &ban_flags, &ban_reason, &ban_kickmsg, &ban_cmd, &ban_source))
    {
        return NULL;
    }
    
#if SOURCE_ENGINE != SE_DARKMESSIAH
    /* Check how we should ban the player */
    if ((ban_flags & BANFLAG_AUTO) == BANFLAG_AUTO)
    {
        if (gamehelpers->IsLANServer() || !player->IsAuthorized())
        {
            ban_flags |= BANFLAG_IP;
            ban_flags &= ~BANFLAG_AUTHID;
        }
        else
        {
            ban_flags |= BANFLAG_AUTHID;
            ban_flags &= ~BANFLAG_IP;
        }
    }
    else if ((ban_flags & BANFLAG_IP) == BANFLAG_IP)
    {
        ban_flags |= BANFLAG_IP;
        ban_flags &= ~BANFLAG_AUTHID;
    }
    else if ((ban_flags & BANFLAG_AUTHID) == BANFLAG_AUTHID)
    {
        if (player->IsAuthorized())
        {
            ban_flags |= BANFLAG_AUTHID;
            ban_flags &= ~BANFLAG_IP;
        }
        else
            Py_RETURN_FALSE;
    }
    else
    {
        PyErr_SetString(g_pViperException, "No valid ban flags specified.");
        return NULL;
    }
    
    cell_t handled = 0;
    if (ban_cmd[0] != '\0' && g_pSMOnBanClient->GetFunctionCount() > 0)
    {
        g_pSMOnBanClient->PushCell(self->index);
        g_pSMOnBanClient->PushCell(ban_time);
        g_pSMOnBanClient->PushCell(ban_flags);
        g_pSMOnBanClient->PushString(ban_reason);
        g_pSMOnBanClient->PushString(ban_cmd);
        g_pSMOnBanClient->PushCell(ban_source);
        g_pSMOnBanClient->Execute(&handled);
        
        if (!handled)
        {
            PyObject *fwd_args = Py_BuildValue("(Oiissi)", self, ban_time,
                ban_flags, ban_reason, ban_cmd, ban_source);
            g_pViperOnBanClient->Execute(&handled, fwd_args);
            Py_DECREF(fwd_args);
        }
    }
#else
	/* Dark Messiah doesn't have Steam IDs so there is only one ban method to choose */
	ban_flags |= BANFLAG_IP;
	ban_flags &= ~BANFLAG_AUTHID;
#endif
    
    if (!handled)
    {
        if ((ban_flags & BANFLAG_IP) == BANFLAG_IP)
        {
            /* Get the IP address and strip the port */
            char ip[24];
            strncopy(ip, player->GetIPAddress(), sizeof(ip));
            
            char *ptr = strchr(ip, ':');
            if (ptr != NULL)
                *ptr = '\0';
            
            char bancommand[256];
            UTIL_Format(bancommand, sizeof(bancommand), "addip %d %s\n", ban_time, ip);
            
            /* Kick... */
            if ((ban_flags & BANFLAG_NOKICK) != BANFLAG_NOKICK)
                player->Kick(ban_kickmsg);
            
            /*  ...then ban */
            engine->ServerCommand(bancommand);
            
            /* Now physically write the ban to file */
            if ((ban_time == BANTIME_FOREVER) &&
                ((ban_flags & BANFLAG_NOWRITE) != BANFLAG_NOWRITE))
            {
                engine->ServerCommand("writeip\n");
            }
        }
        else if ((ban_flags & BANFLAG_AUTHID) == BANFLAG_AUTHID)
        {
            char bancommand[256];
            UTIL_Format(bancommand, sizeof(bancommand), "banid %d %s\n",
                ban_time, player->GetAuthString());
            
            if ((ban_flags & BANFLAG_NOKICK) != BANFLAG_NOKICK)
                player->Kick(ban_kickmsg);
            
            engine->ServerCommand(bancommand);
            
            /* Now physically write the ban to file */
            if ((ban_time == BANTIME_FOREVER) &&
                ((ban_flags & BANFLAG_NOWRITE) != BANFLAG_NOWRITE))
            {
                engine->ServerCommand("writeid\n");
            }
        }
    }
    else if ((ban_flags & BANFLAG_NOKICK) != BANFLAG_NOKICK)
        player->Kick(ban_kickmsg);
    
    Py_RETURN_TRUE;
}

static PyObject *
clients__Client__fake_command(clients__Client *self, PyObject *args)
{
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    
    char *cmd;
    if (!PyArg_ParseTuple(args, "s", &cmd))
        return NULL;
    
    g_pServerPluginHelpers->ClientCommand(player->GetEdict(), cmd);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__is_timing_out(clients__Client *self)
{
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
    else if (player->IsFakeClient())
        return PyErr_Format(g_pViperException, "Client %d is fake", self->index);
    
    INetChannelInfo *pInfo = engine->GetPlayerNetInfo(self->index);
    
    return PyBool_FromLong(pInfo->IsTimingOut());
}

static PyObject *
clients__Client__kick(clients__Client *self, PyObject *args, PyObject *kwds)
{
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    
    char const *msg = "";
    bool delay = true;
    
    static char *kwdlist[] = {"msg", "delay", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|sb", kwdlist, &msg, &delay))
        return NULL;
    
    /* Ignore duplicate kicks */
    if (player->IsInKickQueue())
        Py_RETURN_FALSE;
    
    player->MarkAsBeingKicked();
    
    if (player->IsFakeClient())
    {
        char kickcmd[48];
        unsigned short userid = engine->GetPlayerUserId(player->GetEdict());
        UTIL_Format(kickcmd, sizeof(kickcmd), "kickid %d\n", userid);
        
        engine->ServerCommand(kickcmd);
        
        Py_RETURN_TRUE;
    }
    
    if (delay)
        gamehelpers->AddDelayedKick(self->index, player->GetUserId(), msg);
    else
        player->Kick(msg);
    
    Py_RETURN_TRUE;
}

static PyObject *
clients__Client__notify_post_admin_check(clients__Client *self)
{
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
    else if (!player->IsAuthorized())
        return PyErr_Format(g_pViperException, "Client %d is not authorized", self->index);
    
    player->NotifyPostAdminChecks();
    
    Py_RETURN_NONE;
}

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
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
    
    char *message;
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;
    
    gamehelpers->TextMsg(self->index, HUD_PRINTTALK, message);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__print_console(clients__Client *self, PyObject *args)
{
    IGamePlayer *player = NULL;
    if (self->index != 0)
    {
        player = playerhelpers->GetGamePlayer(self->index);
        if (!player->IsConnected())
            return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        else if (!player->IsInGame())
            return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
        
        /* Silently fail on bots; the engine will crash otherwise */
        if (player->IsFakeClient())
            Py_RETURN_NONE;
    }
    
    char *message;
    Py_ssize_t len;
    if (!PyArg_ParseTuple(args, "s#", &message, &len))
        return NULL;
    
    char buffer[1024];
    strncpy((char*)&buffer, message, len+1)[len] = '\n';
    
    if (self->index != 0)
        engine->ClientPrintf(player->GetEdict(), buffer);
    else
        META_CONPRINT(buffer);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__print_hint(clients__Client *self, PyObject *args)
{
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);

    char const *msg;
    if (!PyArg_ParseTuple(args, "s", &msg))
        return NULL;
    
    if (!g_HL2.HintTextMsg(self->index, msg))
    {
        PyErr_SetString(g_pViperException, "Could not send HintTextMsg usermessage.");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__set_fake_client_convar(clients__Client *self, PyObject *args)
{
    char const *convar;
    char const *value;
    
    if (!PyArg_ParseTuple(args, "ss", &convar, &value))
        return NULL;
    
    if (self->index < 1)
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    
    if (!player->IsFakeClient())
        return PyErr_Format(g_pViperException, "Client %d is not fake", self->index);
    
    edict_t *pEdict = player->GetEdict();
    if (pEdict == NULL)
        return PyErr_Format(g_pViperException, "Unable to retrieve client %d's edict", self->index);
    
    engine->SetFakeClientConVarValue(pEdict, convar, value);
    
    Py_RETURN_NONE;
}

static PyObject *
clients__Client__show_vgui_panel(clients__Client *self, PyObject *args,
                                 PyObject *kwds)
{
    char const *name;
    keyvalues__KeyValues *pykv = NULL;
    KeyValues *kv = NULL;
    bool show = true;
    
    static char *keywdlist[] = {"name", "kv", "show"};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O!b", keywdlist, &name,
        &pykv, &keyvalues__KeyValuesType, &show))
    {
        return NULL;
    }
    
    if (pykv != NULL)
        kv = pykv->kv;
    
    if (!g_HL2.ShowVGUIMenu(self->index, name, kv, show))
    {
        PyErr_SetString(g_pViperException, "Could not send VGUIMenu usermessage");
        return NULL;
    }
    
    Py_RETURN_NONE;
}


/* Get/Sets */
static PyObject *
clients__Client__entityget(clients__Client *self)
{
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return NULL;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
    
    edict_t *pEdict = player->GetEdict();
    if (pEdict == NULL)
        return PyErr_Format(g_pViperException, "Unable to retrieve client %d's edict", self->index);
    
    entity__Entity *entity = (entity__Entity *)entity__EntityType.tp_new(
        &entity__EntityType, NULL, NULL);
    entity->edict = pEdict;

	return (PyObject*)entity;
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
clients__Client__serialget(clients__Client *self)
{
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    return PyInt_FromSsize_t(player->GetSerial());
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
        return PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
        return PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
    else if (!player->IsInGame())
        return PyErr_Format(g_pViperException, "Client %d is not in game", self->index);

	IPlayerInfo *pInfo = player->GetPlayerInfo();
	if (pInfo == NULL)
	{
        PyErr_SetString(g_pViperException, "IPlayerInfo not supported by this game");
        return NULL;
	}

	return PyInt_FromLong(pInfo->GetTeamIndex());
}

static int
clients__Client__teamset(clients__Client *self, PyObject *setobj)
{
    if (!PyInt_Check(setobj))
    {
        PyErr_Format(_PyExc_TypeError, "expected object of type int, found %s",
            setobj->ob_type->tp_name);
        return -1;
    }
    
    if (self->index < 1)
    {
        PyErr_Format(g_pViperException, "Client %d is invalid", self->index);
        return -1;
    }
    
    IGamePlayer *player = playerhelpers->GetGamePlayer(self->index);
    if (!player->IsConnected())
    {
        PyErr_Format(g_pViperException, "Client %d is not connected", self->index);
        return -1;
    }
    else if (!player->IsInGame())
    {
        PyErr_Format(g_pViperException, "Client %d is not in game", self->index);
        return -1;
    }

	IPlayerInfo *pInfo = player->GetPlayerInfo();
	if (pInfo == NULL)
	{
        PyErr_SetString(g_pViperException, "IPlayerInfo not supported by this game");
        return -1;
	}
	
	pInfo->ChangeTeam(PyInt_AS_LONG(setobj));
	
	return 0;
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
    
    return PyInt_FromLong(player->IsFakeClient() ? engine->GetPlayerUserId(player->GetEdict())
                           : player->GetUserId());
}

enum clients__client__getsets_t
{
    CLIENT_GETSET_ABS_ANGLES = 0,
    CLIENT_GETSET_ABS_ORIGIN,
    CLIENT_GETSET_ALIVE,
    CLIENT_GETSET_ARMOR,
    CLIENT_GETSET_AVG_CHOKE,
    CLIENT_GETSET_AVG_CHOKE_IN,
    CLIENT_GETSET_AVG_CHOKE_OUT,
    CLIENT_GETSET_AVG_DATA,
    CLIENT_GETSET_AVG_DATA_IN,
    CLIENT_GETSET_AVG_DATA_OUT,
    CLIENT_GETSET_AVG_LATENCY,
    CLIENT_GETSET_AVG_LATENCY_IN,
    CLIENT_GETSET_AVG_LATENCY_OUT,
    CLIENT_GETSET_AVG_LOSS,
    CLIENT_GETSET_AVG_LOSS_IN,
    CLIENT_GETSET_AVG_LOSS_OUT,
    CLIENT_GETSET_AVG_PACKETS,
    CLIENT_GETSET_AVG_PACKETS_IN,
    CLIENT_GETSET_AVG_PACKETS_OUT,
    CLIENT_GETSET_DATA_RATE,
    CLIENT_GETSET_DEATHS,
    CLIENT_GETSET_FRAGS,
    CLIENT_GETSET_HEALTH,
    CLIENT_GETSET_LATENCY,
    CLIENT_GETSET_LATENCY_IN,
    CLIENT_GETSET_LATENCY_OUT,
    CLIENT_GETSET_MAXS,
    CLIENT_GETSET_MINS,
    CLIENT_GETSET_MODEL,
    CLIENT_GETSET_TIME_CONNECTED,
    CLIENT_GETSET_WEAPON,
};

static PyObject *
clients__Client__getter_valid_ingame_modsupport(clients__Client *self,
                                                clients__client__getsets_t type)
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
	
	switch (type)
	{
	case CLIENT_GETSET_ABS_ANGLES:
	    return CreatePyVector(pInfo->GetAbsAngles());
    case CLIENT_GETSET_ABS_ORIGIN:
        return CreatePyVector(pInfo->GetAbsOrigin());
    case CLIENT_GETSET_ALIVE:
        return PyBool_FromLong(!pInfo->IsDead());
    case CLIENT_GETSET_ARMOR:
        return PyInt_FromLong(pInfo->GetArmorValue());
    case CLIENT_GETSET_DEATHS:
        return PyInt_FromLong(pInfo->GetDeathCount());
    case CLIENT_GETSET_FRAGS:
        return PyInt_FromLong(pInfo->GetFragCount());
    case CLIENT_GETSET_HEALTH:
        return PyInt_FromLong(pInfo->GetHealth());
    case CLIENT_GETSET_MAXS:
        return CreatePyVector(pInfo->GetPlayerMaxs());
    case CLIENT_GETSET_MINS:
        return CreatePyVector(pInfo->GetPlayerMins());
    case CLIENT_GETSET_MODEL:
        return PyString_FromString(pInfo->GetModelName());
    case CLIENT_GETSET_WEAPON:
        return PyString_FromString(pInfo->GetWeaponName());
	}
	
	Py_RETURN_NONE;
}

static PyObject *
clients__Client__getter_valid_ingame_fake(clients__Client *self,
                                          clients__client__getsets_t type)
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
    
    if (player->IsFakeClient())
    {
        PyErr_Format(g_pViperException, "Client %d is fake", self->index);
        return NULL;
    }
    
    INetChannelInfo *pInfo = engine->GetPlayerNetInfo(self->index);
    
    switch (type)
    {
    case CLIENT_GETSET_AVG_CHOKE:
        return PyFloat_FromDouble(pInfo->GetAvgChoke(FLOW_INCOMING) +
                                  pInfo->GetAvgChoke(FLOW_OUTGOING));
    case CLIENT_GETSET_AVG_CHOKE_IN:
        return PyFloat_FromDouble(pInfo->GetAvgChoke(FLOW_INCOMING));
    case CLIENT_GETSET_AVG_CHOKE_OUT:
        return PyFloat_FromDouble(pInfo->GetAvgChoke(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_AVG_DATA:
        return PyFloat_FromDouble(pInfo->GetAvgData(FLOW_INCOMING) +
                                  pInfo->GetAvgData(FLOW_OUTGOING));
    case CLIENT_GETSET_AVG_DATA_IN:
        return PyFloat_FromDouble(pInfo->GetAvgData(FLOW_INCOMING));
    case CLIENT_GETSET_AVG_DATA_OUT:
        return PyFloat_FromDouble(pInfo->GetAvgData(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_AVG_LATENCY:
        return PyFloat_FromDouble(pInfo->GetAvgLatency(FLOW_INCOMING) +
                                  pInfo->GetAvgLatency(FLOW_OUTGOING));
    case CLIENT_GETSET_AVG_LATENCY_IN:
        return PyFloat_FromDouble(pInfo->GetAvgLatency(FLOW_INCOMING));
    case CLIENT_GETSET_AVG_LATENCY_OUT:
        return PyFloat_FromDouble(pInfo->GetAvgLatency(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_AVG_LOSS:
        return PyFloat_FromDouble(pInfo->GetAvgLoss(FLOW_INCOMING) +
                                  pInfo->GetAvgLoss(FLOW_OUTGOING));
    case CLIENT_GETSET_AVG_LOSS_IN:
        return PyFloat_FromDouble(pInfo->GetAvgLoss(FLOW_INCOMING));
    case CLIENT_GETSET_AVG_LOSS_OUT:
        return PyFloat_FromDouble(pInfo->GetAvgLoss(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_AVG_PACKETS:
        return PyFloat_FromDouble(pInfo->GetAvgPackets(FLOW_INCOMING) +
                                  pInfo->GetAvgPackets(FLOW_OUTGOING));
    case CLIENT_GETSET_AVG_PACKETS_IN:
        return PyFloat_FromDouble(pInfo->GetAvgPackets(FLOW_INCOMING));
    case CLIENT_GETSET_AVG_PACKETS_OUT:
        return PyFloat_FromDouble(pInfo->GetAvgPackets(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_DATA_RATE:
        return PyInt_FromLong(pInfo->GetDataRate());
    
    
    case CLIENT_GETSET_LATENCY:
        return PyFloat_FromDouble(pInfo->GetLatency(FLOW_INCOMING) +
                                  pInfo->GetLatency(FLOW_OUTGOING));
    case CLIENT_GETSET_LATENCY_IN:
        return PyFloat_FromDouble(pInfo->GetLatency(FLOW_INCOMING));
    case CLIENT_GETSET_LATENCY_OUT:
        return PyFloat_FromDouble(pInfo->GetLatency(FLOW_OUTGOING));
    
    
    case CLIENT_GETSET_TIME_CONNECTED:
        return PyFloat_FromDouble(pInfo->GetTimeConnected());
    }
    
    Py_RETURN_NONE;
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
                self->index, player->IsFakeClient() ?
                engine->GetPlayerUserId(player->GetEdict())
                : player->GetUserId(), player->GetName(),
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
    {"abs_angles", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's angles vector.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_ABS_ANGLES},
    {"abs_origin", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's origin vector.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_ABS_ORIGIN},
    {"alive", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "Whether the client is alive or dead.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_ALIVE},
    {"armor", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's armor value.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_ARMOR},
    {"avg_choke", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average packet choke. Value is a percentage ranging from 0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_CHOKE},
    {"avg_choke_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average incoming packet choke. Value is a percentage ranging from\n"
        "0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_CHOKE_IN},
    {"avg_choke_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average outgoing packet choke. Value is a percentage ranging from\n"
        "0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_CHOKE_OUT},
    {"avg_data", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's data flow, incoming and outgoing, in bytes per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_AVG_DATA},
    {"avg_data_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's incoming data flow in bytes per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_AVG_DATA_IN},
    {"avg_data_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's outgoing data flow in bytes per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_AVG_DATA_OUT},
    {"avg_latency", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average packet latency, both incoming and outgoing, in seconds.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LATENCY},
    {"avg_latency_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average incoming packet latency in seconds.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LATENCY_IN},
    {"avg_latency_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average outgoing packet latency in seconds.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LATENCY_OUT},
    {"avg_loss", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average packet loss, incoming and outgoing. Value is a percentage\n"
        "ranging from 0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LOSS},
    {"avg_loss_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average incoming packet loss. Value is a percentage ranging from"
        "0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LOSS_IN},
    {"avg_loss_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average outgoing packet loss. Value is a percentage ranging from\n"
        "0.0 to 1.0\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_LOSS_OUT},
    {"avg_packets", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average packet frequency, incoming and outgoing, in packets per\n"
        "second.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_PACKETS},
    {"avg_packets_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average incoming packet frequency in packets per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_PACKETS_IN},
    {"avg_packets_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's average outgoing packet frequency in packets per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_AVG_PACKETS_OUT},
    {"data_rate", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's send data rate in bytes per second.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_DATA_RATE},
    {"deaths", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's death count.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_DEATHS},
    {"entity", (getter)clients__Client__entityget, NULL,
        "The Entity of the client.\n"
        "@throw ViperError: Invalid client, client not connected, or client not in-game."},
    {"fake", (getter)clients__Client__fakeget, NULL,
        "Whether the client is fake or not.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"frags", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's frag count.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_FRAGS},
    {"health", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The health of the client.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_HEALTH},
    {"ip", (getter)clients__Client__ipget, NULL,
        "The IP address of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"lang_id", (getter)clients__Client__lang_idget, NULL,
        "This client's language id.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"latency", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's packet latency (RTT), both incoming and outgoing, in seconds.\n"
        "This is more accurate than the average latency, but jittery.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_LATENCY},
    {"latency_in", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's incoming packet latency (RTT) in seconds.\n"
        "This is more accurate than the average latency, but jittery.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_LATENCY_IN},
    {"latency_out", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The client's outgoing packet latency (RTT) in seconds.\n"
        "This is more accurate than the average latency, but jittery.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_LATENCY_OUT},
    {"maxs", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's max size vector.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_MAXS},
    {"mins", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's min size vector.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_MINS},
    {"model", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's model name.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_MODEL},
    {"name", (getter)clients__Client__nameget, NULL,
        "The name of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"serial", (getter)clients__Client__serialget, NULL,
        "The client's unique serial identifier."},
    {"steamid", (getter)clients__Client__steamidget, NULL,
        "The Steam ID of this client. This is None when the client is not\n"
        "authorized yet.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"team", (getter)clients__Client__teamget, (setter)clients__Client__teamset,
        "This player's team.\n"
        "@throw ViperError: Invalid client, client not connected, or client not in-game."},
    {"time_connected", (getter)clients__Client__getter_valid_ingame_fake, NULL,
        "The amount of time the client has been connected in seconds.\n"
        "@throw ViperError: Invalid client, client not in-game, or fake client.",
        (void *)CLIENT_GETSET_TIME_CONNECTED},
    {"userid", (getter)clients__Client__useridget, NULL,
        "The userid of this client.\n"
        "@throw ViperError: Invalid client or client not connected."},
    {"weapon", (getter)clients__Client__getter_valid_ingame_modsupport, NULL,
        "The client's weapon name.\n"
        "@throw ViperError: Invalid client, client not in-game, or no mod support.",
        (void *)CLIENT_GETSET_WEAPON},
    {NULL},
};

static PyMethodDef clients__Client__methods[] = {
#ifdef NOT_DOCUMENTED_YET
    {"get_info",}, // Should this be renamed? It's misleading
    {"kick",}, // Be sure to implement delay, and delay=False
    
    /* TODO: All admin stuff
     * XXX: Should "user" be removed from the names?
     *      YES! There should be a separate Privileges object containing all admin stuff
     */
    {"set_user_admin",},
    {"add_user_flags",},
    {"remove_user_flags",},
    {"get_user_flag_bits",},
    {"set_user_flag_bits",},
    {"",},
    {"UNDEFINED",},
    {"UNDEFINED",},
    
    {"is_in_kick_queue",},
    {"can_user_target",},
    {"is_timing_out",},
    {"UNDEFINED",},
    {"UNDEFINED",},
    {"UNDEFINED",},
    {"ban", (PyCFunction)clients__Client__ban, METH_VARARGS | METH_KEYWORDS,
        "ban(time, flags, reason, kickmsg=\"Kicked\"[, cmd=None[, source=0]]]) -> bool\n\n"
        "Bans the client.\n\n",
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
#endif
    {"fake_command", (PyCFunction)clients__Client__fake_command, METH_VARARGS,
        "fake_command(cmd)\n\n"
        "Executes a client command on the server without being networked.\n\n"
        "@type  cmd: str\n"
        "@param cmd: The command string to execute."},
    {"is_timing_out", (PyCFunction)clients__Client__is_timing_out, METH_NOARGS,
        "is_timing_out() -> bool\n\n"
        "Returns if the client is timing out.\n\n"
        "@rtype: bool\n"
        "@return: True if the client is timing out, False otherwise."},
    {"kick", (PyCFunction)clients__Client__kick, METH_VARARGS|METH_KEYWORDS,
        "kick([msg=""[, delay=True]])\n\n"
        "Disconnects a player from a server.\n\n"
        "@type  msg: str\n"
        "@param msg: A message to show the user as a disconnect reason. Note that a\n"
        "   period is automatically appended to the message by the engine.\n"
        "@type  delay: bool\n"
        "@param delay: If True, the client is kicked in the next game frame. If False,\n"
        "   the client is kicked immediately. The delay exists to prevent accidental\n"
        "   engine crashes."},
    {"notify_post_admin_check", (PyCFunction)clients__Client__notify_post_admin_check, METH_VARARGS,
        "notify_post_admin_check()\n\n"
        "Signals that a player has completed post-connection admin checks. Has no effect\n"
        "if the player has already had this event signalled. Note: This must be sent even\n"
        "if no admin id was assigned."},
    {"print_center", (PyCFunction)clients__Client__print_center, METH_VARARGS,
        "print_center(message)\n\n"
        "Prints a message to this client in the center of the screen.\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"print_console", (PyCFunction)clients__Client__print_console, METH_VARARGS,
        "print_console(message)\n\n"
        "Prints a message to this client's console.\n\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"print_chat", (PyCFunction)clients__Client__print_chat, METH_VARARGS,
        "print_chat(message)\n\n"
        "Prints a message to this client's chat area.\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"print_hint", (PyCFunction)clients__Client__print_hint, METH_VARARGS,
        "print_hint(message)\n\n"
        "Prints a message to this client with a hint box.\n\n"
        "@type  message: str\n"
        "@param message: The message to print"},
    {"set_fake_client_convar", (PyCFunction)clients__Client__set_fake_client_convar, METH_VARARGS,
        "set_fake_client_convar(convar, value)\n\n"
        "Sets a convar value on a fake client.\n"
        "@type  convar: str\n"
        "@param convar: The ConVar name.\n"
        "@type  value: str\n"
        "@param value: The value to set the ConVar to."},
    {"show_vgui_panel", (PyCFunction)clients__Client__show_vgui_panel, METH_VARARGS|METH_KEYWORDS,
        "show_vgui_panel(name[, kv=None[, show=True]])\n\n"
        "Shows a VGUI panel to a specific client\n\n"
        "@type  name: str\n"
        "@param name: Panel type name (see viewport_panel_names.h for a list of panel\n"
        "   names)\n"
        "@type  kv: sourcemod.keyvalues.KeyValues\n"
        "@param kv: A KeyValues with all the data for the panel setup. The data inside\n"
        "   the KeyValues depends on the panel type.\n"
        "@type  show: bool\n"
        "@param show: True to show the panel, or False to remove it from the client's\n"
        "    screen"},
    {NULL, NULL, 0, NULL}
};

PyTypeObject clients__ClientType = {
    PyObject_HEAD_INIT(_PyType_Type)
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
    
    return g_Players.GetPythonClient(IndexOfEdict(pEdict));
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
    
    return g_Players.GetPythonClient(index);
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
clients__get_client_from_serial(PyObject *self, PyObject *args)
{
    unsigned int serial;
    if (!PyArg_ParseTuple(args, "n", &serial))
        return NULL;
    
    int index = playerhelpers->GetClientFromSerial(serial);
    if (index == 0)
        Py_RETURN_NONE;
    
    return g_Players.GetPythonClient(index);
}


static PyObject *
clients__get_client_from_userid(PyObject *self, PyObject *args)
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
    
    return g_Players.GetPythonClient(index);
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
    {"get_client_from_serial", clients__get_client_from_serial, METH_VARARGS,
        "get_client_from_serial(serial) -> Client object\n\n"
        "Retrieves a Client from its serial number.\n\n"
        "@type  serial: int\n"
        "@param serial: The Client's serial number\n"
        "@rtype: sourcemod.clients.Client\n"
        "@return: A valid Client object on success, None if an invalid serial."},
    {"get_client_from_userid", clients__get_client_from_userid, METH_VARARGS,
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
    Py_INCREF(_PyType_Type);
    clients__ClientType.ob_type = _PyType_Type;
    if (PyType_Ready(&clients__ClientType) < 0)
        return NULL;
    
    PyObject *clients = Py_InitModule3("clients", clients__methods,
        "Contains objects and methods to manipulate and handle clients.");
    
    Py_INCREF((PyObject*)&clients__ClientType);
    PyModule_AddObject(clients, "Client", (PyObject*)&clients__ClientType);
    
    /* TODO: Move these to sourcemod.banning module */
    PyModule_AddIntMacro(clients, BANFLAG_AUTO);
    PyModule_AddIntMacro(clients, BANFLAG_IP);
    PyModule_AddIntMacro(clients, BANFLAG_AUTHID);
    PyModule_AddIntMacro(clients, BANFLAG_NOKICK);
    PyModule_AddIntMacro(clients, BANFLAG_NOWRITE);
    PyModule_AddIntMacro(clients, BANTIME_FOREVER);
    
    return clients;
}


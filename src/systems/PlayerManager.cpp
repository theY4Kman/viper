/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008 Zach "theY4Kman" Kanzler
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

ViperPlayerManager::ViperPlayerManager()
{
    m_SingleClientArgs = PyTuple_Pack(1, &clients__ClientType);
}

void
ViperPlayerManager::OnViperStartup(bool late)
{
}

void
ViperPlayerManager::OnClientConnected(int client)
{
}

void
ViperPlayerManager::OnClientPutInServer(int client)
{
}

void
ViperPlayerManager::OnClientDisconnecting(int client)
{
}

void
ViperPlayerManager::OnClientDisconnected(int client)
{
}

void
ViperPlayerManager::OnClientAuthorized(int client)
{
}

bool
ViperPlayerManager::OnClientPreAdminCheck(int client)
{
    return true;
}

void
ViperPlayerManager::OnClientPostAdminCheck(int client)
{
}


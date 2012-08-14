/**
 * =============================================================================
 * Viper
 * Copyright (C) 2010 Brandon "monokrome" Stoner
 * Copyright (C) 2012 PimpinJuice
 * Copyright (C) 2007-2012 Zach "theY4Kman" Kanzler
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

#ifndef _INCLUDE_VIPER_PLAYERMANAGER_H_
#define _INCLUDE_VIPER_PLAYERMANAGER_H_

#include "viper_globals.h"
#include <IPlayerHelpers.h>
#include <IViperPluginSys.h>
#include "ForwardSys.h"
#include "extension.h"

namespace Viper {
	extern IViperForward *g_pViperOnBanClient;
	extern SourceMod::IForward *g_pSMOnBanIdentity;
	extern SourceMod::IForward *g_pSMOnBanClient;

	ViperResultType InterceptClientConnectCallback(IViperForward *fwd, PyObject *ret,
												   IViperPluginFunction *func);

	class ViperPlayerManager :
		public ViperGlobalClass,
		public SourceMod::IClientListener
	{
	public:
		ViperPlayerManager();
	#if SOURCE_ENGINE >= SE_ORANGEBOX
		void OnClientCommand(edict_t *pEntity, const CCommand &args);
	#else
		void OnClientCommand(edict_t *pEntity);
	#endif
	public: // ViperGlobalClass
		virtual void OnViperStartup(bool late);
		virtual void OnViperShutdown();
	public: // IClientListener
		virtual bool InterceptClientConnect(int client, char *reject, size_t maxrejectlen);
		virtual void OnClientConnected(int client);
		virtual void OnClientPutInServer(int client);
		virtual void OnClientDisconnecting(int client);
		virtual void OnClientDisconnected(int client);
		virtual void OnClientAuthorized(int client, char const *authstring);
    
		virtual bool OnClientPreAdminCheck(int client);
		virtual void OnClientPostAdminCheck(int client);
    
		virtual void OnServerActivate(int clientMax);
	public:
		/**
		 * @brief Returns the sourcemod.clients.Client object for that client index
		 * @note: This returns a NEW REFERENCE
		 */
		PyObject *GetPythonClient(int client);

	private:
		IViperForward *m_OnClientConnect;
		IViperForward *m_OnClientConnected;
		IViperForward *m_OnClientPutInServer;
		IViperForward *m_OnClientDisconnecting;
		IViperForward *m_OnClientDisconnected;
		IViperForward *m_OnClientAuthorized;
    
		IViperForward *m_OnClientPreAdminCheck;
		IViperForward *m_OnClientPostAdminCheck;
    
		IViperForward *m_OnServerActivate;
		IViperForward *m_OnMapStart;
    
		PyObject **m_Clients;
	};

	extern ViperPlayerManager g_Players;
}

#endif // _INCLUDE_VIPER_PLAYERMANAGER_H_


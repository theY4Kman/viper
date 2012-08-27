#ifndef __INCLUDE_FORWARDSCLIENTLISTENER_H__
#define __INCLUDE_FORWARDSCLIENTLISTENER_H__

#include "STL.h"
#include <IPlayerHelpers.h>

class ForwardsClientListener : public SourceMod::IClientListener {
public:
	bool InterceptClientConnect(int clientIndex, char *error, size_t maxlength);
	void OnClientConnected(int clientIndex);
	void OnClientPutInServer(int clientIndex);
	void OnClientDisconnecting(int clientIndex);
	void OnClientDisconnected(int clientIndex);
	void OnClientAuthorized(int clientIndex, const char *authstring);
	void OnServerActivated(int max_clients);
	bool OnClientPreAdminCheck(int clientIndex);
	void OnClientPostAdminCheck(int clientIndex);
	void OnMaxPlayersChanged(int newvalue);
	void OnClientSettingsChanged(int clientIndex);
};

#endif
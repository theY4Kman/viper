#include "SDKToolsClientListener.h"
#include "BoostPythonSM.h"
#include "SDKToolsModule.h"
#include "Util.h"
#include "sdk/smsdk_ext.h"

namespace py = boost::python;

void SDKToolsClientListener::OnClientDisconnecting(int client) {
	int max_clients = playerhelpers->GetMaxClients();

	if (sdktools__VoiceHookCount == 0)
	{
		return;
	}

	/* This can probably be optimized more, but I doubt it's that much 
	 * of an actual bottleneck.
	 */

	/* Reset other clients who receive from or have muted this client */
	for (int i = 1; i <= max_clients; i++)
	{
		if (i == client)
		{
			continue;
		}

		sdktools__ClientMutes[i][client] = false;
		sdktools__ClientMutes[client][i] = false;
		
		if (sdktools__VoiceMap[i][client] != Listen_Default)
		{
			sdktools__VoiceMap[i][client] = Listen_Default;
			if (sdktools__VoiceDecHookCount())
			{
				break;
			}
		}
		if (sdktools__VoiceMap[client][i] != Listen_Default)
		{
			sdktools__VoiceMap[client][i] = Listen_Default;
			if (sdktools__VoiceDecHookCount())
			{
				break;
			}
		}
	}

	if (sdktools__VoiceFlags[client])
	{
		sdktools__VoiceFlags[client] = SPEAK_NORMAL;
		sdktools__VoiceDecHookCount();
	}
}

SDKToolsClientListener sdktools__ClientListener;
#ifndef __INCLUDE_SDKTOOLSCLIENTLISTENER_H__
#define __INCLUDE_SDKTOOLSCLIENTLISTENER_H__

#include "STL.h"
#include <IPlayerHelpers.h>

class SDKToolsClientListener : public SourceMod::IClientListener {
public:
	void OnClientDisconnecting(int clientIndex);
};

extern SDKToolsClientListener sdktools__ClientListener;

#endif
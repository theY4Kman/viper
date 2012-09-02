#ifndef __INCLUDE_FORWARDSMODULE_H__
#define __INCLUDE_FORWARDSMODULE_H__

#include "Macros.h"
#include "STL.h"
#include "ForwardType.h"
#include "ForwardsClientListener.h"

extern "C" __declspec(dllexport) void initForwards();
extern void destroyForwards();

extern bool forwards__GameFrameCallback(boost::python::object returnValue);
extern bool forwards__ClientConnectCallback(boost::python::object returnValue);

extern std::map<std::string, ForwardType*> forwards__GlobalForwards;
extern ForwardType *forwards__GameFrameForward;
extern ForwardType *forwards__ClientConnectForward;
extern ForwardType *forwards__ClientConnectedForward;
extern ForwardType *forwards__ClientPutInServerForward;
extern ForwardType *forwards__ClientDisconnectingForward;
extern ForwardType *forwards__ClientDisconnectedForward;
extern ForwardType *forwards__ClientAuthorizedForward;
extern ForwardType *forwards__ServerActivatedForward;
extern ForwardType *forwards__ClientPreAdminCheckForward;
extern ForwardType *forwards__ClientPostAdminCheckForward;
extern ForwardType *forwards__MaxPlayersChangedForward;
extern ForwardType *forwards__ClientSettingsChangedForward;

extern ForwardsClientListener *forwards_ClientListener;

extern bool forwards_ClientConnect_RejectConnection;
extern std::string forwards_ClientConnect_RejectMessage;

extern bool forwards_ClientPreAdminCheck_ReturnValue;

extern void forwards__register(std::string globalForwardName, boost::python::object callbackFunction);

extern void forwards__GameFrame(bool simulating);

#endif
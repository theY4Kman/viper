#ifndef __INCLUDE_CLIENTSMODULE_H__
#define __INCLUDE_CLIENTSMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initClients();
extern void destroyClients();
extern void unloadThreadStateClients(PyThreadState *threadState);

DEFINE_CUSTOM_EXCEPTION_DECL(ClientIndexOutOfRangeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientDataNotAvailableExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotConnectedExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotInGameExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotFakeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientIsFakeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidUserIDExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidClientSerialExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotAuthorizedExceptionType, Clients)

extern void clients__GameFrame(bool simulating);

extern int clients__LifeStateOffset;

#endif
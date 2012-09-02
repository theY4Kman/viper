#ifndef __INCLUDE_CLIENTSMODULE_H__
#define __INCLUDE_CLIENTSMODULE_H__

#include "Macros.h"
#include "STL.h"

extern "C" __declspec(dllexport) void initClients();
extern void destroyClients();

DEFINE_CUSTOM_EXCEPTION_DECL(ClientIndexOutOfRangeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientDataNotAvailableExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotConnectedExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotInGameExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotFakeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientIsFakeExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidUserIdExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidClientSerialExceptionType, Clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotAuthorizedExceptionType, Clients)

extern void clients__GameFrame(bool simulating);

extern std::map<int, std::string> clients__KickQueue;

extern int clients__LifeStateOffset;

#endif